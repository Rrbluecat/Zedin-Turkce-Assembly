#include <stdio.h>

FILE *okuma_dosyasi = NULL;
FILE *yazma_dosyasi = NULL;

/* ── DOSYA_AC: okuma icin dosya acar ── */
int DOSYA_AC(int *ram, int adres) {
    char isim[128];
    int i = 0;
    while (ram[adres + i] != 0 && i < 127) {
        isim[i] = (char)ram[adres + i];
        i++;
    }
    isim[i] = '\0';
    if (okuma_dosyasi) fclose(okuma_dosyasi);
    okuma_dosyasi = fopen(isim, "r");
    return (okuma_dosyasi != NULL) ? 1 : 0;
}

/* ── DOSYA_YAZ_AC: yazma icin dosya acar ── */
int DOSYA_YAZ_AC(int *ram, int adres) {
    char isim[128];
    int i = 0;
    while (ram[adres + i] != 0 && i < 127) {
        isim[i] = (char)ram[adres + i];
        i++;
    }
    isim[i] = '\0';
    if (yazma_dosyasi) fclose(yazma_dosyasi);
    yazma_dosyasi = fopen(isim, "wb");
    return (yazma_dosyasi != NULL) ? 1 : 0;
}

/* ── DOSYA_OKU: dosyadan bir karakter okur, EOF'ta -1 ── */
int DOSYA_OKU(void) {
    if (!okuma_dosyasi) return -1;
    int ch = fgetc(okuma_dosyasi);
    return (ch == EOF) ? -1 : ch;
}

/* ── DOSYA_YAZ: dosyaya bir karakter yazar ── */
void DOSYA_YAZ(int karakter) {
    if (yazma_dosyasi) fputc((char)karakter, yazma_dosyasi);
}

/* ── DOSYA_KAPAT: tum dosyalari kapatir ── */
void DOSYA_KAPAT(void) {
    if (okuma_dosyasi) { fclose(okuma_dosyasi); okuma_dosyasi = NULL; }
    if (yazma_dosyasi) { fclose(yazma_dosyasi); yazma_dosyasi = NULL; }
}
