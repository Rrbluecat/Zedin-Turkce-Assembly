#include <stdio.h>
#include "lib_io.c"

int ram[8192] = {0};

int main(void) {
    /* Test 1: YAZI_YAZ */
    ram[100] = 'M'; ram[101] = 'e'; ram[102] = 'r';
    ram[103] = 'h'; ram[104] = 'a'; ram[105] = 'b';
    ram[106] = 'a'; ram[107] = '!'; ram[108] = 0;
    printf("Test 1 - YAZI_YAZ: ");
    YAZI_YAZ(ram, 100);
    printf("\n");

    /* Test 2: SAYI_YAZ */
    printf("Test 2 - SAYI_YAZ: ");
    SAYI_YAZ(42);
    printf("\n");

    /* Test 3: KATAR_OKU */
    printf("Test 3 - KATAR_OKU (bir şey yaz): ");
    KATAR_OKU(ram, 200, 50);
    printf("Okunan: ");
    YAZI_YAZ(ram, 200);
    printf("\n");

    printf("\nTum testler tamam!\n");
    return 0;
}
