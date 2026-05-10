#include <stdio.h>#include <string.h>#include <stdlib.h>#include "zed_opcodes.h"#include "libzed.h"#define BELLEK_BOYUTU 8192#define MAX_SEMBOL    500#define MAX_MODUL     32typedef struct { char isim[30]; int adres; } Sembol;typedef struct {    char dosya[128];    int  kod_baslangic;    int  kod_uzunlugu;    Sembol etiketler[MAX_SEMBOL];    int    e_say;    Sembol degiskenler[MAX_SEMBOL];    int    d_say;} Modul;Modul moduller[MAX_MODUL];int modul_say = 0;int birlesik_bellek[BELLEK_BOYUTU];int birlesik_ram[BELLEK_BOYUTU];int toplam_kod = 0;int komut_bul(char *ad) {    for (int i = 0; i < ZED_KOMUT_SAYISI; i++)        if (strcmp(ad, ZED_KOMUT_SETI[i].isim) == 0) return i;    return -1;}int etiket_bul_global(char *ad) {    for (int m = 0; m < modul_say; m++)        for (int i = 0; i < moduller[m].e_say; i++)            if (strcmp(moduller[m].etiketler[i].isim, ad) == 0)                return moduller[m].etiketler[i].adres;    return -1;}int degisken_al_modul(Modul *m, char *ad) {    for (int i = 0; i < m->d_say; i++)        if (strcmp(m->degiskenler[i].isim, ad) == 0) return i;    strcpy(m->degiskenler[m->d_say].isim, ad);    return m->d_say++;}void ram_string_yaz_link(int adres, const char *metin) {    int i = 0, j = 0;    while (metin[i] != '\0') {        if (metin[i] != '"' && metin[i] != ',')
            birlesik_ram[adres + j++] = (int)metin[i];
        i++;
    }
    birlesik_ram[adres + j] = 0;
}

void hata(const char *mesaj) {
    printf("\033[0;31m[LINK HATA]: %s\033[0m\n", mesaj);
    exit(1);
}

void modul_gecis_1(Modul *mod) {
    FILE *f = fopen(mod->dosya, "r");
    if (!f) { printf("[HATA] %s bulunamadi!\n", mod->dosya); exit(1); }
    char m[128];
    int pos = mod->kod_baslangic;
    while (fscanf(f, "%127s", m) != EOF) {
        if (m[0] == ';') { fgets(m, sizeof(m), f); continue; }
        if (m[0] == '@') {
            strcpy(mod->etiketler[mod->e_say].isim, m + 1);
            mod->etiketler[mod->e_say++].adres = pos;
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
    fclose(f);
    mod->kod_uzunlugu = pos - mod->kod_baslangic;
}

void modul_gecis_2(Modul *mod) {
    FILE *f = fopen(mod->dosya, "r");
    if (!f) { printf("[HATA] %s bulunamadi!\n", mod->dosya); exit(1); }
    char m[128];
    int yaz = mod->kod_baslangic;
    while (fscanf(f, "%127s", m) != EOF) {
        if (m[0] == ';') { fgets(m, sizeof(m), f); continue; }
        if (m[0] == '@') continue;
        if (strcmp(m, "DATA") == 0) {
            int hedef; char metin[128];
            fscanf(f, "%d %127s", &hedef, metin);
            ram_string_yaz_link(hedef, metin);
            continue;
        }
        int k = komut_bul(m);
        if (k == -1) {
            printf("\033[0;33m[UYARI] Bilinmeyen komut '%s'\033[0m\n", m);
            continue;
        }
        birlesik_bellek[yaz++] = ZED_KOMUT_SETI[k].op;
        for (int i = 0; i < ZED_KOMUT_SETI[k].boyut - 1; i++) {
            fscanf(f, "%127s", m);
            if (m[0] == '#')
                birlesik_bellek[yaz++] = atoi(m + 1);
            else if (m[0] == '$') {
                int adres = etiket_bul_global(m + 1);
                if (adres == -1) hata("Bilinmeyen etiket!");
                birlesik_bellek[yaz++] = adres;
            }
            else if ((m[0] >= '0' && m[0] <= '9') || m[0] == '-')
                birlesik_bellek[yaz++] = atoi(m);
            else
                birlesik_bellek[yaz++] = degisken_al_modul(mod, m);
        }
    }
    fclose(f);
}

void bin_yaz(const char *hedef) {
    int ram_son = 3000;
    for (int i = BELLEK_BOYUTU - 1; i >= 0; i--) {
        if (birlesik_ram[i] != 0) { ram_son = i + 1; break; }
    }
    if (ram_son < 3000) ram_son = 3000;
    FILE *bin = fopen(hedef, "wb");
    if (!bin) { printf("[HATA] %s yazılamadı!\n", hedef); exit(1); }
    int magic = ZED_MAGIC, ram_bas = 0;
    fwrite(&magic,          sizeof(int), 1,          bin);
    fwrite(&toplam_kod,     sizeof(int), 1,          bin);
    fwrite(&ram_bas,        sizeof(int), 1,          bin);
    fwrite(&ram_son,        sizeof(int), 1,          bin);
    fwrite(birlesik_bellek, sizeof(int), toplam_kod, bin);
    fwrite(birlesik_ram,    sizeof(int), ram_son,    bin);
    fclose(bin);
    printf("\033[0;32m✓ Linklendi → %s\033[0m\n", hedef);
    printf("  Modul sayisi : %d\n", modul_say);
    printf("  Toplam kod   : %d int (%d byte)\n", toplam_kod, toplam_kod * 4);
    printf("  Toplam RAM   : %d int (%d byte)\n", ram_son, ram_son * 4);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Kullanim: zed_link <cikti.bin> <modul1.zed> [modul2.zed ...]\n");
        return 1;
    }
    const char *hedef = argv[1];
    int kod_offset = 0;
    for (int i = 2; i < argc; i++) {
        if (modul_say >= MAX_MODUL) hata("Cok fazla modul!");
        Modul *mod = &moduller[modul_say++];
        memset(mod, 0, sizeof(Modul));
        strcpy(mod->dosya, argv[i]);
        mod->kod_baslangic = kod_offset;
        modul_gecis_1(mod);
        kod_offset += mod->kod_uzunlugu;
        printf("  Modul: %-20s | Kod: %d-%d\n",
               mod->dosya, mod->kod_baslangic,
               mod->kod_baslangic + mod->kod_uzunlugu - 1);
    }
    toplam_kod = kod_offset;
    for (int i = 0; i < modul_say; i++)
        modul_gecis_2(&moduller[i]);
    bin_yaz(hedef);
    return 0;
}
