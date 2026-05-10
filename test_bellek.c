#include <stdio.h>
#include "lib_bellek.c"

int ram[8192] = {0};

int main(void) {
    /* Test 1: Normal ayırma */
    printf("Test 1 - BELLEK_AYIR:\n");
    int adres1 = BELLEK_AYIR(100);
    int adres2 = BELLEK_AYIR(200);
    printf("Blok 1 adresi: %d\n", adres1);
    printf("Blok 2 adresi: %d\n", adres2);
    BELLEK_DURUM();

    /* Test 2: Boşalt */
    printf("\nTest 2 - BELLEK_BOSALT:\n");
    BELLEK_BOSALT(ram);
    BELLEK_DURUM();

    /* Test 3: Taşma kontrolü */
    printf("\nTest 3 - Tasma:\n");
    int adres3 = BELLEK_AYIR(99999);
    printf("Sonuc: %d (-1 olmali)\n", adres3);

    printf("\nTum testler tamam!\n");
    return 0;
}
