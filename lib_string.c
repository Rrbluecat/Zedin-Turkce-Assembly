#include <stdio.h>

/* ── KATAR_UZUNLUK: RAM'deki string uzunluğunu döner ── */
int KATAR_UZUNLUK(int *ram, int adres) {
    int uzunluk = 0;
    while (ram[adres + uzunluk] != 0) {
        uzunluk++;
    }
    return uzunluk;
}

/* ── KATAR_KOPYALA: RAM'de bir string'i başka adrese kopyalar ── */
void KATAR_KOPYALA(int *ram, int kaynak, int hedef) {
    int i = 0;
    while (ram[kaynak + i] != 0) {
        ram[hedef + i] = ram[kaynak + i];
        i++;
    }
    ram[hedef + i] = 0;
}

/* ── SAYI_KATARA: int'i RAM'deki string'e çevirir ── */
void SAYI_KATARA(int *ram, int sayi, int adres) {
    char tampon[32];
    int i = 0, j = 0;
    if (sayi < 0) { ram[adres + j++] = '-'; sayi = -sayi; }
    if (sayi == 0) { ram[adres + j++] = '0'; ram[adres + j] = 0; return; }
    while (sayi > 0) { tampon[i++] = '0' + (sayi % 10); sayi /= 10; }
    while (i > 0) { ram[adres + j++] = tampon[--i]; }
    ram[adres + j] = 0;
}
