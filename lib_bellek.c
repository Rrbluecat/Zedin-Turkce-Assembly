#include <stdio.h>

#define BELLEK_BOYUTU 8192

int heap_baslangic = 3000;
int heap_isaretci  = 3000;

/* ── BELLEK_AYIR: heap'ten blok ayırır, başlangıç adresini döner ── */
int BELLEK_AYIR(int boyut) {
    if (heap_isaretci + boyut >= BELLEK_BOYUTU) {
        printf("[HATA] BELLEK_AYIR: Yeterli bellek yok!\n");
        return -1;
    }
    int adres = heap_isaretci;
    heap_isaretci += boyut;
    return adres;
}

/* ── BELLEK_BOSALT: heap'i sıfırlar ── */
void BELLEK_BOSALT(int *ram) {
    for (int i = heap_baslangic; i < heap_isaretci; i++) {
        ram[i] = 0;
    }
    heap_isaretci = heap_baslangic;
}

/* ── BELLEK_DURUM: kaç byte kullanıldığını gösterir ── */
void BELLEK_DURUM(void) {
    int kullanilan = heap_isaretci - heap_baslangic;
    int kalan = BELLEK_BOYUTU - heap_isaretci;
    printf("Kullanilan: %d | Kalan: %d\n", kullanilan, kalan);
}
