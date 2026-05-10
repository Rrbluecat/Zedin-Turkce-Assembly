#include <stdio.h>
#include "lib_string.c"

int ram[8192] = {0};

int main(void) {
    /* Test 1: KATAR_UZUNLUK */
    ram[100]='M'; ram[101]='e'; ram[102]='r';
    ram[103]='h'; ram[104]='a'; ram[105]='b';
    ram[106]='a'; ram[107]=0;
    printf("Test 1 - KATAR_UZUNLUK: %d (7 olmali)\n",
           KATAR_UZUNLUK(ram, 100));

    /* Test 2: KATAR_KOPYALA */
    KATAR_KOPYALA(ram, 100, 200);
    printf("Test 2 - KATAR_KOPYALA: ");
    int i = 200;
    while (ram[i]) { printf("%c", (char)ram[i]); i++; }
    printf("\n");

    /* Test 3: SAYI_KATARA */
    SAYI_KATARA(ram, 12345, 300);
    printf("Test 3 - SAYI_KATARA pozitif: ");
    i = 300; while (ram[i]) { printf("%c", (char)ram[i]); i++; }
    printf("\n");

    SAYI_KATARA(ram, -99, 400);
    printf("Test 3 - SAYI_KATARA negatif: ");
    i = 400; while (ram[i]) { printf("%c", (char)ram[i]); i++; }
    printf("\n");

    SAYI_KATARA(ram, 0, 500);
    printf("Test 3 - SAYI_KATARA sifir: ");
    i = 500; while (ram[i]) { printf("%c", (char)ram[i]); i++; }
    printf("\n");

    printf("\nTum testler tamam!\n");
    return 0;
}
