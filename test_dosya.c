#include <stdio.h>
#include "lib_dosya.c"

int ram[8192] = {0};

int main(void) {
    /* Test 1: Yazma */
    ram[100]='t'; ram[101]='e'; ram[102]='s';
    ram[103]='t'; ram[104]='.'; ram[105]='t';
    ram[106]='x'; ram[107]='t'; ram[108]=0;

    printf("Test 1 - DOSYA_YAZ_AC: ");
    int sonuc = DOSYA_YAZ_AC(ram, 100);
    printf("%s\n", sonuc ? "Acildi" : "Hata");

    DOSYA_YAZ('Z'); DOSYA_YAZ('E'); DOSYA_YAZ('D');
    DOSYA_YAZ('\n');
    DOSYA_KAPAT();

    /* Test 2: Okuma */
    printf("Test 2 - DOSYA_AC ve DOSYA_OKU: ");
    DOSYA_AC(ram, 100);
    int ch;
    while ((ch = DOSYA_OKU()) != -1) {
        if (ch != '\n') printf("%c", (char)ch);
    }
    printf("\n");
    DOSYA_KAPAT();

    printf("\nTum testler tamam!\n");
    return 0;
}
