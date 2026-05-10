#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── YAZI_YAZ: RAM'deki string'i ekrana basar ── */
void YAZI_YAZ(int *ram, int adres) {
    while (ram[adres] != 0) {
        printf("%c", (char)ram[adres]);
        adres++;
    }
}

/* ── SAYI_YAZ: int'i ekrana basar ── */
void SAYI_YAZ(int sayi) {
    printf("%d", sayi);
}

/* ── SAYI_OKU: klavyeden int okur ── */
int SAYI_OKU(void) {
    int sayi = 0;
    scanf("%d", &sayi);
    return sayi;
}

/* ── KATAR_OKU: klavyeden string okur, RAM'e yazar ── */
void KATAR_OKU(int *ram, int adres, int max) {
    char girdi[256];
    if (fgets(girdi, sizeof(girdi), stdin)) {
        int i = 0;
        while (girdi[i] && girdi[i] != '\n' && i < max - 1) {
            ram[adres + i] = (int)girdi[i];
            i++;
        }
        ram[adres + i] = 0;
    }
}
