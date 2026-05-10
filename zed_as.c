#include <stdio.h>#include <string.h>#include <stdlib.h>#include "zed_opcodes.h"#include "libzed.h"#define BELLEK_BOYUTU 8192#define MAX_SEMBOL    500typedef struct { char isim[30]; int adres; } Sembol;Sembol etiketler[MAX_SEMBOL];Sembol degiskenler[MAX_SEMBOL];int e_say = 0, d_say = 0;int sanal_bellek[BELLEK_BOYUTU];int ram[BELLEK_BOYUTU];int kod_uzunlugu = 0;int komut_bul(char *ad) {    for (int i = 0; i < ZED_KOMUT_SAYISI; i++)        if (strcmp(ad, ZED_KOMUT_SETI[i].isim) == 0) return i;    return -1;}int etiket_bul(char *ad) {    for (int i = 0; i < e_say; i++)        if (strcmp(etiketler[i].isim, ad) == 0) return etiketler[i].adres;    return -1;}int degisken_al(char *ad) {    for (int i = 0; i < d_say; i++)        if (strcmp(degiskenler[i].isim, ad) == 0) return i;    strcpy(degiskenler[d_say].isim, ad);    return d_say++;}void ram_string_yaz(int adres, const char *metin) {    int i = 0, j = 0;    while (metin[i] != '\0') {        if (metin[i] != '"' && metin[i] != ',')            ram[adres + j++] = (int)metin[i];        i++;    }    ram[adres + j] = 0;}void hata(const char *mesaj, int satir) {    printf("\033[0;31m[HATA] Satir %d: %s\033[0m\n", satir, mesaj);    exit(1);}void gecis_1(FILE *f) {    char m[128];    int pos = 0, satir = 0;    while (fscanf(f, "%127s", m) != EOF) {        satir++;        if (m[0] == ';') { fgets(m, sizeof(m), f); continue; }        if (m[0] == '@') {            if (e_say >= MAX_SEMBOL) hata("Cok fazla etiket!", satir);            strcpy(etiketler[e_say].isim, m + 1);            etiketler[e_say++].adres = pos;            continue;        }        if (strcmp(m, "DATA") == 0) {            int adr; char val[128];            fscanf(f, "%d %127s", &adr, val);            continue;        }        int k = komut_bul(m);        if (k != -1) pos += ZED_KOMUT_SETI[k].boyut;    }}void gecis_2(FILE *f) {    char m[128];    int yaz = 0, satir = 0;    while (fscanf(f, "%127s", m) != EOF) {        satir++;        if (m[0] == ';') { fgets(m, sizeof(m), f); continue; }        if (m[0] == '@') continue;        if (strcmp(m, "DATA") == 0) {            int hedef; char metin[128];            fscanf(f, "%d %127s", &hedef, metin);            ram_string_yaz(hedef, metin);            continue;        }        int k = komut_bul(m);        if (k == -1) {            printf("\033[0;33m[UYARI] Satir %d: Bilinmeyen komut '%s'\033[0m\n", satir, m);            continue;        }        sanal_bellek[yaz++] = ZED_KOMUT_SETI[k].op;        for (int i = 0; i < ZED_KOMUT_SETI[k].boyut - 1; i++) {
            fscanf(f, "%127s", m);
            if (m[0] == '#')
                sanal_bellek[yaz++] = atoi(m + 1);
            else if (m[0] == '$') {
                int adres = etiket_bul(m + 1);
                if (adres == -1) hata("Bilinmeyen etiket!", satir);
                sanal_bellek[yaz++] = adres;
            }
            else if ((m[0] >= '0' && m[0] <= '9') || m[0] == '-')
                sanal_bellek[yaz++] = atoi(m);
            else
                sanal_bellek[yaz++] = degisken_al(m);
        }
    }
    kod_uzunlugu = yaz;
}

void bin_yaz(const char *hedef) {
    int ram_son = 3000;
    for (int i = BELLEK_BOYUTU - 1; i >= 0; i--) {
        if (ram[i] != 0) { ram_son = i + 1; break; }
    }
    if (ram_son < 3000) ram_son = 3000;
    FILE *bin = fopen(hedef, "wb");
    if (!bin) { printf("[HATA] %s yazılamadı!\n", hedef); exit(1); }
    int magic = ZED_MAGIC, ram_bas = 0;
    fwrite(&magic,        sizeof(int), 1,            bin);
    fwrite(&kod_uzunlugu, sizeof(int), 1,            bin);
    fwrite(&ram_bas,      sizeof(int), 1,            bin);
    fwrite(&ram_son,      sizeof(int), 1,            bin);
    fwrite(sanal_bellek,  sizeof(int), kod_uzunlugu, bin);
    fwrite(ram,           sizeof(int), ram_son,      bin);
    fclose(bin);
    printf("\033[0;32m✓ Derlendi → %s\033[0m\n", hedef);
    printf("  Kod : %d int (%d byte)\n", kod_uzunlugu, kod_uzunlugu * 4);
    printf("  RAM : %d int (%d byte)\n", ram_son, ram_son * 4);
}

int main(int argc, char *argv[]) {
    if (argc < 2) { printf("Kullanim: zed_as <kaynak.zed> [cikti.bin]\n"); return 1; }
    const char *kaynak = argv[1];
    const char *hedef  = (argc >= 3) ? argv[2] : "program.bin";
    FILE *f = fopen(kaynak, "r");
    if (!f) { printf("[HATA] %s bulunamadi!\n", kaynak); return 1; }
    gecis_1(f); rewind(f); gecis_2(f); fclose(f);
    bin_yaz(hedef);
    return 0;
}
