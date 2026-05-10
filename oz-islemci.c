#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "zed_opcodes.h"

#define BELLEK_BOYUTU 8192
#define STACK_BOYUTU  512
#define MAX_SEMBOL    500
#define ZED_MAGIC     0x5A454400  /* "ZED\0" */

/* ──────────────────────────────────────────
   GLOBAL DURUM
   ────────────────────────────────────────── */
typedef struct { char isim[30]; int adres; } Sembol;
Sembol degiskenler[MAX_SEMBOL], etiketler[MAX_SEMBOL];
int d_say = 0, e_say = 0, ip = 0, sp = 0;
int sanal_bellek[BELLEK_BOYUTU] = {0};
int ram[BELLEK_BOYUTU]          = {0};
int stack[STACK_BOYUTU];

FILE *dosya_okuma = NULL;
FILE *dosya_yazma = NULL;

int heap_baslangic = 3000, heap_isaretci = 3000;

/* ──────────────────────────────────────────
   YARDIMCI FONKSİYONLAR
   ────────────────────────────────────────── */
void hata(const char *msg) {
    printf("\033[0;31m\n[HATA]: %s (IP: %d, SP: %d)\033[0m\n", msg, ip, sp);
    if (dosya_okuma) fclose(dosya_okuma);
    if (dosya_yazma) fclose(dosya_yazma);
    exit(1);
}

int komut_bul(char *ad) {
    for (int i = 0; i < ZED_KOMUT_SAYISI; i++)
        if (strcmp(ad, ZED_KOMUT_SETI[i].isim) == 0) return i;
    return -1;
}

const char* op_isim_bul(int op) {
    for (int i = 0; i < ZED_KOMUT_SAYISI; i++)
        if (ZED_KOMUT_SETI[i].op == op) return ZED_KOMUT_SETI[i].isim;
    return "BILINMEYEN";
}

int etiket_bul(char *ad) {
    for (int i = 0; i < e_say; i++)
        if (strcmp(etiketler[i].isim, ad) == 0) return etiketler[i].adres;
    return -1;
}

int degisken_al(char *ad) {
    for (int i = 0; i < d_say; i++)
        if (strcmp(degiskenler[i].isim, ad) == 0) return i;
    strcpy(degiskenler[d_say].isim, ad);
    return d_say++;
}

void ram_string_al(int adres, char *hedef, int max) {
    int i = 0;
    while (i < max - 1 && ram[adres + i] != 0) {
        hedef[i] = (char)ram[adres + i];
        i++;
    }
    hedef[i] = '\0';
}

/* ──────────────────────────────────────────
   PARSER: kod.txt → sanal_bellek + ram
   ────────────────────────────────────────── */
int parse_kaynak(const char *dosya_adi, int *kod_uzunlugu) {
    FILE *f = fopen(dosya_adi, "r");
    if (!f) { printf("HATA: %s bulunamadi!\n", dosya_adi); return 0; }

    char m[128];
    int pos = 0;

    /* 1. Geçiş: Etiket adreslerini hesapla */
    while (fscanf(f, "%127s", m) != EOF) {
        if (m[0] == ';') { fgets(m, sizeof(m), f); continue; }
        if (m[0] == '@') {
            strcpy(etiketler[e_say].isim, m + 1);
            etiketler[e_say++].adres = pos;
            continue;
        }
        if (strcmp(m, "DATA") == 0) {
            int adr; char val[128];
            fscanf(f, "%d %127s", &adr, val);
            continue;
        }
        int k = komut_bul(m);
        if (k != -1) pos += ZED_KOMUT_SETI[k].boyut;
    }
    rewind(f);

    /* 2. Geçiş: Belleği doldur */
    int yaz = 0;
    while (fscanf(f, "%127s", m) != EOF) {
        if (m[0] == ';') { fgets(m, sizeof(m), f); continue; }
        if (m[0] == '@')  continue;

        if (strcmp(m, "DATA") == 0) {
            int hedef_adres;
            char metin[128];
            fscanf(f, "%d %127s", &hedef_adres, metin);
            int j = 0;
            for (int i = 0; metin[i] != '\0'; i++) {
                if (metin[i] != '"' && metin[i] != ',')
                    ram[hedef_adres + j++] = (int)metin[i];
            }
            ram[hedef_adres + j] = 0;
            continue;
        }

        int k = komut_bul(m);
        if (k != -1) {
            sanal_bellek[yaz++] = ZED_KOMUT_SETI[k].op;
            for (int i = 0; i < ZED_KOMUT_SETI[k].boyut - 1; i++) {
                fscanf(f, "%127s", m);
                if (m[0] == '#')
                    sanal_bellek[yaz++] = atoi(m + 1);
                else if (m[0] == '$')
                    sanal_bellek[yaz++] = etiket_bul(m + 1);
                else if ((m[0] >= '0' && m[0] <= '9') || m[0] == '-')
                    sanal_bellek[yaz++] = atoi(m);
                else
                    sanal_bellek[yaz++] = degisken_al(m);
            }
        }
    }
    fclose(f);
    *kod_uzunlugu = yaz;
    return 1;
}

/* ──────────────────────────────────────────
   DERLE: kod.txt → .bin
   
   Binary format:
     [4 byte] ZED_MAGIC
     [4 byte] kod_uzunlugu  (int sayısı)
     [4 byte] ram_baslangic (her zaman 0)
     [4 byte] ram_uzunlugu  (int sayısı)
     [kod_uzunlugu * 4 byte] sanal_bellek
     [ram_uzunlugu * 4 byte] ram
   ────────────────────────────────────────── */
void derle(const char *kaynak, const char *hedef) {
    int kod_uzunlugu = 0;
    if (!parse_kaynak(kaynak, &kod_uzunlugu)) return;

    /* RAM'in anlamlı kısmını bul */
    int ram_son = heap_baslangic;
    for (int i = BELLEK_BOYUTU - 1; i >= 0; i--) {
        if (ram[i] != 0) { ram_son = i + 1; break; }
    }
    if (ram_son < heap_baslangic) ram_son = heap_baslangic;

    FILE *bin = fopen(hedef, "wb");
    if (!bin) { printf("HATA: %s yazma hatasi!\n", hedef); return; }

    int magic     = ZED_MAGIC;
    int ram_bas   = 0;
    fwrite(&magic,          sizeof(int), 1,              bin);
    fwrite(&kod_uzunlugu,   sizeof(int), 1,              bin);
    fwrite(&ram_bas,        sizeof(int), 1,              bin);
    fwrite(&ram_son,        sizeof(int), 1,              bin);
    fwrite(sanal_bellek,    sizeof(int), kod_uzunlugu,   bin);
    fwrite(ram,             sizeof(int), ram_son,        bin);

    fclose(bin);

    printf("\033[0;32m✓ Derlendi → %s\033[0m\n", hedef);
    printf("  Kod : %d int (%d byte)\n", kod_uzunlugu, kod_uzunlugu * 4);
    printf("  RAM : %d int (%d byte)\n", ram_son,      ram_son      * 4);
    printf("  Toplam: %d byte\n", (4 + kod_uzunlugu + ram_son) * 4);
}

/* ──────────────────────────────────────────
   BIN YÜKLE: .bin → sanal_bellek + ram
   ────────────────────────────────────────── */
int bin_yukle(const char *dosya_adi) {
    FILE *bin = fopen(dosya_adi, "rb");
    if (!bin) { printf("HATA: %s bulunamadi!\n", dosya_adi); return 0; }

    int magic, kod_uzunlugu, ram_bas, ram_uzunlugu;
    fread(&magic,         sizeof(int), 1, bin);
    fread(&kod_uzunlugu,  sizeof(int), 1, bin);
    fread(&ram_bas,       sizeof(int), 1, bin);
    fread(&ram_uzunlugu,  sizeof(int), 1, bin);

    if (magic != ZED_MAGIC) {
        printf("\033[0;31mHATA: Gecersiz ZED binary! (Magic: 0x%X)\033[0m\n", magic);
        fclose(bin);
        return 0;
    }

    fread(sanal_bellek, sizeof(int), kod_uzunlugu, bin);
    fread(ram,          sizeof(int), ram_uzunlugu,  bin);
    fclose(bin);

    printf("\033[0;32m✓ Yuklendi: %s\033[0m\n", dosya_adi);
    printf("  Kod: %d int | RAM: %d int\n", kod_uzunlugu, ram_uzunlugu);
    return 1;
}

/* ──────────────────────────────────────────
   ÇALIŞTIR
   ────────────────────────────────────────── */
void calistir(int debug_mod, int baslangic_ip) {
    int ceb = 0, aktif = 1;
    ip = baslangic_ip; sp = 0;

    if (baslangic_ip == 0) printf("--- OZ-ISLEMCI v32.1 ---\n");

    while (aktif) {
        if (ip < 0 || ip >= BELLEK_BOYUTU) hata("IP bellek disina cikti!");
        int op = sanal_bellek[ip];

        if (debug_mod) {
            printf("\033[1;33m[DBG] IP:%03d | OP:%-14s | CEB:%-6d | SP:%d\033[0m\n",
                   ip, op_isim_bul(op), ceb, sp);
            (void)getchar();
        }

        switch (op) {
            case YUKLE:      ceb = sanal_bellek[ip+1]; ip += 2; break;
            case SAKLA:      ram[sanal_bellek[ip+2]] = sanal_bellek[ip+1]; ip += 3; break;
            case GETIR:      ceb = ram[sanal_bellek[ip+1]]; ip += 2; break;
            case SAKLA_CEBI: ram[sanal_bellek[ip+1]] = ceb; ip += 2; break;

            case TOPLA: ceb += sanal_bellek[ip+1]; ip += 2; break;
            case CIKAR: ceb -= sanal_bellek[ip+1]; ip += 2; break;
            case CARP:  ceb *= sanal_bellek[ip+1]; ip += 2; break;
            case BOL:
                if (sanal_bellek[ip+1] == 0) hata("Sifira bolme!");
                ceb /= sanal_bellek[ip+1]; ip += 2; break;
            case MOD:
                if (sanal_bellek[ip+1] == 0) hata("Sifira mod!");
                ceb %= sanal_bellek[ip+1]; ip += 2; break;

            case VE:    ceb &= sanal_bellek[ip+1]; ip += 2; break;
            case VEYA:  ceb |= sanal_bellek[ip+1]; ip += 2; break;
            case DEGIL: ceb = ~ceb; ip++; break;

            case GIT:          ip = sanal_bellek[ip+1]; break;
            case EGER_ESITSE:  ip = (ceb == ram[sanal_bellek[ip+1]]) ? sanal_bellek[ip+2] : ip+3; break;
            case EGER_DEGILSE: ip = (ceb != ram[sanal_bellek[ip+1]]) ? sanal_bellek[ip+2] : ip+3; break;
            case BUYUKSE:      ip = (ceb >  ram[sanal_bellek[ip+1]]) ? sanal_bellek[ip+2] : ip+3; break;
            case KUCUKSE:      ip = (ceb <  ram[sanal_bellek[ip+1]]) ? sanal_bellek[ip+2] : ip+3; break;

            case PUSH:
                if (sp >= STACK_BOYUTU) hata("Stack Overflow");
                stack[sp++] = sanal_bellek[ip+1]; ip += 2; break;
            case POP:
                if (sp <= 0) hata("Stack Underflow");
                ceb = stack[--sp]; ip++; break;
            case CALL:
                if (sp >= STACK_BOYUTU) hata("Stack Overflow (CALL)");
                stack[sp++] = ip + 2;
                ip = sanal_bellek[ip+1]; break;
            case RET:
                if (sp <= 0) hata("Stack Underflow (RET)");
                ip = stack[--sp]; break;

            case SAKLA_IND:
                ram[ram[sanal_bellek[ip+1]] + ram[sanal_bellek[ip+2]]] = ceb; ip += 3; break;
            case GETIR_IND:
                ceb = ram[ram[sanal_bellek[ip+1]] + ram[sanal_bellek[ip+2]]]; ip += 3; break;
            case SAKLA_B:
                ram[ram[sanal_bellek[ip+1]] + ram[sanal_bellek[ip+2]]] = (ceb & 0xFF); ip += 3; break;
            case GETIR_B:
                ceb = ram[ram[sanal_bellek[ip+1]] + ram[sanal_bellek[ip+2]]] & 0xFF; ip += 3; break;

            case OKU: {
                int hedef = sanal_bellek[ip+1];
                char girdi[128];
                if (fgets(girdi, sizeof(girdi), stdin)) {
                    int i = 0;
                    while (girdi[i] && girdi[i] != '\n' && i < 126) {
                        ram[hedef + i] = (int)girdi[i]; i++;
                    }
                    ram[hedef + i] = 0;
                    ceb = i;
                }
                ip += 2; break;
            }

            case KESME: {
                int c = sanal_bellek[ip+1];
                char ds[128];
                switch (c) {
                    case 20: printf("%c", (char)ceb); break;
                    case 21: { int ch = getchar(); ceb = ch; } break;
                    case 22: { int ptr = ceb; while (ram[ptr]) { printf("%c",(char)ram[ptr]); ptr++; } } break;
                    case 23: printf("%d", ceb); break;

                    case 25: ceb = (ceb <= 0) ? rand() : rand() % ceb; break;
                    case 26: ceb = (int)time(NULL); break;

                    case 30:
                        ram_string_al(ceb, ds, sizeof(ds));
                        if (dosya_okuma) { fclose(dosya_okuma); dosya_okuma = NULL; }
                        dosya_okuma = fopen(ds, "r");
                        ceb = (dosya_okuma != NULL) ? 1 : 0;
                        break;
                    case 31:
                        if (dosya_okuma) {
                            int ch = fgetc(dosya_okuma);
                            ceb = (ch == EOF) ? -1 : ch;
                        } else { ceb = -1; }
                        break;

                    case 32:
                        ram_string_al(ceb, ds, sizeof(ds));
                        if (dosya_yazma) { fclose(dosya_yazma); dosya_yazma = NULL; }
                        dosya_yazma = fopen(ds, "wb"); /* binary yazma */
                        ceb = (dosya_yazma != NULL) ? 1 : 0;
                        break;
                    case 33:
                        if (dosya_yazma) fputc((char)ceb, dosya_yazma);
                        break;
                    case 34:
                        if (dosya_okuma) { fclose(dosya_okuma); dosya_okuma = NULL; }
                        if (dosya_yazma) { fclose(dosya_yazma); dosya_yazma = NULL; }
                        break;
                    case 35:
                        if (dosya_yazma) {
                            int ptr = ceb;
                            while (ram[ptr]) { fputc((char)ram[ptr], dosya_yazma); ptr++; }
                        }
                        break;

                    case 40: {
                        int boyut = ceb;
                        ceb = heap_isaretci;
                        heap_isaretci += boyut;
                        if (heap_isaretci >= BELLEK_BOYUTU) hata("OUT OF MEMORY!");
                        break;
                    }
                    case 41: heap_isaretci = heap_baslangic; break;

                    case 45: {
                        int m_boyut  = ceb;
                        int m_kaynak = stack[--sp];
                        int m_hedef  = stack[--sp];
                        for (int i = 0; i < m_boyut; i++) ram[m_hedef+i] = ram[m_kaynak+i];
                        break;
                    }
                    case 46: {
                        int addr2 = stack[--sp];
                        int addr1 = ceb;
                        int esit = 1, i = 0;
                        while (1) {
                            if (ram[addr1+i] != ram[addr2+i]) { esit = 0; break; }
                            if (ram[addr1+i] == 0) break;
                            i++;
                        }
                        ceb = esit;
                     break;
                     }
                    case 50: {
                        char bin_dosya[128];
                        ram_string_al(ceb, bin_dosya, sizeof(bin_dosya));
                        FILE *bin = fopen(bin_dosya, "rb");
                        if (!bin) { printf("[HATA] %s bulunamadi!\n", bin_dosya); ceb = 0; break; }
                        int alt_magic, alt_kod_uz, alt_ram_bas, alt_ram_son;
                        fread(&alt_magic,   sizeof(int), 1, bin);
                        fread(&alt_kod_uz,  sizeof(int), 1, bin);
                        fread(&alt_ram_bas, sizeof(int), 1, bin);
                        fread(&alt_ram_son, sizeof(int), 1, bin);
                        if (alt_magic != ZED_MAGIC) {
                            printf("[HATA] Gecersiz ZED binary!\n");
                            fclose(bin); ceb = 0; break;
                        }
                        int kayit_ip = ip + 2;
                        int kayit_sp = sp;
                        int kayit_ceb = ceb;
                        int kayit_bellek[BELLEK_BOYUTU];
                        int kayit_ram[BELLEK_BOYUTU];
                        memcpy(kayit_bellek, sanal_bellek, sizeof(int) * BELLEK_BOYUTU);
                        memcpy(kayit_ram,    ram,          sizeof(int) * BELLEK_BOYUTU);
                        memset(sanal_bellek, 0, sizeof(int) * BELLEK_BOYUTU);
                        memset(ram,          0, sizeof(int) * BELLEK_BOYUTU);
                        fread(sanal_bellek, sizeof(int), alt_kod_uz,  bin);
                        fread(ram,          sizeof(int), alt_ram_son, bin);
                        fclose(bin);
                        calistir(debug_mod, 0);
                        memcpy(sanal_bellek, kayit_bellek, sizeof(int) * BELLEK_BOYUTU);
                        memcpy(ram,          kayit_ram,    sizeof(int) * BELLEK_BOYUTU);
                        ip  = kayit_ip;
                        sp  = kayit_sp;
                        ceb = kayit_ceb;
                        break;
                    }
                    default:
                        printf("\n[!] Tanimsiz kesme: %d (IP: %d)\n", c, ip);
                        break;
                }
                ip += 2; break;
            }

            case YAZDIR: printf("\nCIKTI: %d\n", ceb); ip++; break;
            case DUR:
                aktif = 0;
                if (dosya_okuma) { fclose(dosya_okuma); dosya_okuma = NULL; }
                if (dosya_yazma) { fclose(dosya_yazma); dosya_yazma = NULL; }
                printf("\nVM Durduruldu.\n");
                break;
            default:
                printf("\n[!] Gecersiz Opcode: %d (IP: %d)\n", op, ip);
                aktif = 0;
                break;
        }
    }
}

/* ──────────────────────────────────────────
   KULLANIM MESAJI
   ────────────────────────────────────────── */
void kullanim(void) {
    printf("Kullanim:\n");
    printf("  oz-islemci                         kod.txt calistir\n");
    printf("  oz-islemci -d                      kod.txt debug mod\n");
    printf("  oz-islemci -derle [cikti.bin]      kod.txt derle\n");
    printf("  oz-islemci -calistir <dosya.bin>   binary calistir\n");
    printf("  oz-islemci -calistir <dosya.bin> -d  debug mod\n");
}

/* ──────────────────────────────────────────
   MAIN
   ────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));

    if (argc == 1) {
        int k = 0;
        if (!parse_kaynak("kod.txt", &k)) return 1;
        calistir(0, 0);
        return 0;
    }

    if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        int k = 0;
        if (!parse_kaynak("kod.txt", &k)) return 1;
        calistir(1, 0);
        return 0;
    }

    if (strcmp(argv[1], "-derle") == 0) {
        const char *hedef = (argc >= 3) ? argv[2] : "program.bin";
        derle("kod.txt", hedef);
        return 0;
    }

    if (strcmp(argv[1], "-calistir") == 0) {
        if (argc < 3) { kullanim(); return 1; }
        if (!bin_yukle(argv[2])) return 1;
        int debug_mod = (argc >= 4 && strcmp(argv[3], "-d") == 0) ? 1 : 0;
        calistir(debug_mod, 0);
        return 0;
    }

    kullanim();
    return 1;
}

