# Zedin Türkçe Assembly ve VM

**Türkçe komut setli, bytecode VM'li, kendi assembler ve linker'ına sahip işletim sistemi dili**

> 10. sınıf öğrencisi tarafından sıfırdan geliştirildi. Termux'ta. Telefondan.
> 
> 

---

## Ne Bu?

Zedin Türkçe Assembly, tamamen Türkçe komutlarla yazılan bir assembly dili ve bu dili çalıştıran bir sanal makinedir. Kendi assembler'ı, linker'ı, standart kütüphanesi ve çekirdeği ile birlikte gelir. Uzun vadeli hedef, altyapısı tamamen Türkçe olan bir işletim sistemi geliştirmektir.
; Merhaba Dünya - Zedin Assembly
DATA 100 "Merhaba_Dunya"
@BASLA
YUKLE #100
INT #22
YUKLE #10
INT #20
BITIR
Dosya uzantısı: `.zed`

---

## Özellikler

| Özellik | Durum | Açıklama |
| --- | --- | --- |
| 🇹🇷 Türkçe Komut Seti | ✅ | YUKLE, SAKLA, TOPLA, CIKAR, GIT, EGER_ESITSE... |
| ⚡ Bytecode VM | ✅ | .zed → .bin → çalıştır |
| 🔧 Türkçe Assembler | ✅ | zed_as: kaynak → binary |
| 🔗 Linker | ✅ | Çoklu modül birleştirme |
| 📚 libZED | ✅ | IO, bellek, string, dosya kütüphaneleri |
| 🖥️ Kernel | ✅ | Türkçe komut satırı çekirdeği |
| 📦 Binary Format | ✅ | ZED magic number ile doğrulamalı .bin formatı |
| 🔄 Program Yükleme | ✅ | Kernel içinden program çalıştırma ve geri dönme |
| 🚀 **Donanım Boot** | ✅ | **SeaBIOS sonrası doğrudan donanım (x86) kontrolü** |

---

## Araç Zinciri

kaynak.zed → [zed_as] → program.bin → [oz-islemci] → çalışır
modul1.zed
modul2.zed → [zed_link] → birlesik.bin → [oz-islemci] → çalışır
modul3.zed
**Donanım Hattı:** kaynak.zed → [zederleyici.py] → çekirdek.bin + [onyukleyici.asm] → tam_zedos.img → **Gerçek Donanım**

---

## Kurulum

```bash
gcc -Wall -o oz-islemci oz-islemci.c
gcc -Wall -o zed_as zed_as.c
gcc -Wall -o zed_link zed_link.c
# Bootloader Derleme
nasm -f bin onyukleyici.asm -o zedos.bin

```

Linux, Ubuntu ve Termux (Android) üzerinde test edildi.
Kullanım

# Kaynak kodu derle

./zed_as program.zed program.bin

# Binary çalıştır

./oz-islemci -calistir program.bin

# Debug modda çalıştır

./oz-islemci -calistir program.bin -d

# Çoklu modül linkle

./zed_link cikti.bin modul1.zed modul2.zed

# Direkt kaynak çalıştır

./oz-islemci

# Donanım (Bootloader) Testi

python3 zederleyici.py
cat zedos.bin cekirdek.bin > tam_zedos.img
qemu-system-x86_64 -drive format=raw,file=tam_zedos.img

Kernel
ZedinVM'in çekirdeği Türkçe assembly ile yazılmıştır.
./zed_as kernel.zed kernel.bin
./oz-islemci -calistir kernel.bin
--- OZ-ISLEMCI v32.1 ---
ZED_OS_v1.0

> > yardim
> > Komutlar:*calistir_yardim_temizle_surum_cikis
> > surum
> > ZED_OS_v1.0*-_Surum:_1.0
> > calistir
> > Dosya_adi: program.bin
> > ...
> > temizle
> > cikis
> > Kernel Komutları
> > Komut
> > Açıklama
> > calistir
> > Program yükle ve çalıştır
> > yardim
> > Komut listesini göster
> > temizle
> > Ekranı temizle
> > surum
> > Versiyon bilgisini göster
> > cikis
> > Sistemi kapat
> > Komut Seti
> > Komut
> > Boyut
> > Açıklama
> > YUKLE #deger
> > 2
> > CEB'e değer yükle
> > SAKLA #deger #adres
> > 3
> > RAM'e yaz
> > GETIR #adres
> > 2
> > RAM'den oku
> > SAKLA_CEBI #adres
> > 2
> > CEB'i RAM'e yaz
> > TOPLA #deger
> > 2
> > CEB += deger
> > CIKAR #deger
> > 2
> > CEB -= deger
> > CARP #deger
> > 2
> > CEB *= deger
> > BOL #deger
> > 2
> > CEB /= deger
> > MOD #deger
> > 2
> > CEB %= deger
> > GIT $etiket
> > 2
> > Etiket adresine atla
> > EGER_ESITSE #adres $etiket
> > 3
> > CEB == RAM[adres] ise atla
> > EGER_DEGILSE #adres $etiket
> > 3
> > CEB != RAM[adres] ise atla
> > BUYUKSE #adres $etiket
> > 3
> > CEB > RAM[adres] ise atla
> > KUCUKSE #adres $etiket
> > 3
> > CEB < RAM[adres] ise atla
> > PUSH #deger
> > 2
> > Stack'e it
> > POP
> > 1
> > Stack'ten al
> > CALL $etiket
> > 2
> > Fonksiyon çağır
> > RET
> > 1
> > Fonksiyondan dön
> > INT #kesme
> > 2
> > Sistem kesmesi
> > YAZDIR
> > 1
> > CEB'i ekrana bas
> > BITIR
> > 1
> > Programı durdur
> > **AKTAR Yazmac, #deger**
> > 3
> > Donanım yazmacına (Y1-Y4) değer yükle
> > **FONK #no**
> > 2
> > Donanım fonksiyonu (Grafik, Yazı)
> > **ZIPLA $adres**
> > 2
> > Kod içinde adrese atla (Donanım)
> > **DUR**
> > 1
> > İşlemciyi durdur (HLT)
> 
> 

Kesme Tablosu (INT)
Kesme
Açıklama
INT #20
CEB'deki ASCII karakteri bas
INT #21
Klavyeden karakter oku
INT #22
RAM'deki string'i bas
INT #23
CEB'deki sayıyı bas
INT #25
Rastgele sayı üret
INT #26
Zaman damgası al
INT #30
Dosya aç (okuma)
INT #31
Dosyadan karakter oku
INT #32
Dosya aç (yazma)
INT #33
Dosyaya karakter yaz
INT #34
Dosyaları kapat
INT #35
Dosyaya string yaz
INT #40
Heap'ten bellek ayır
INT #41
Heap'i sıfırla
INT #46
String karşılaştır
INT #50
Program yükle ve çalıştır
libZED Standart Kütüphane
#include "libzed.h"

YAZI_YAZ(ram, adres)        // RAM'deki string'i ekrana bas
SAYI_YAZ(sayi)              // int'i ekrana bas
SAYI_OKU()                  // klavyeden int oku
KATAR_OKU(ram, adres, max)  // klavyeden string oku

BELLEK_AYIR(boyut)          // heap'ten blok ayır
BELLEK_BOSALT(ram)          // heap'i sıfırla
BELLEK_DURUM()              // bellek durumunu göster

KATAR_UZUNLUK(ram, adres)   // string uzunluğu
KATAR_KOPYALA(ram, kay, hdf)// string kopyala
SAYI_KATARA(ram, sayi, adr) // int → string

DOSYA_AC(ram, adres)        // okuma için dosya aç
DOSYA_YAZ_AC(ram, adres)    // yazma için dosya aç
DOSYA_OKU()                 // dosyadan karakter oku
DOSYA_YAZ(karakter)         // dosyaya karakter yaz
DOSYA_KAPAT()               // dosyaları kapat
Binary Format
[4 byte] ZED Magic (0x5A454400)
[4 byte] Kod uzunluğu (int sayısı)
[4 byte] RAM başlangıcı
[4 byte] RAM uzunluğu
[N*4 byte] Kod (sanal bellek)
[M*4 byte] RAM verisi
Yol Haritası
[x] Türkçe komut seti ve VM
[x] Bytecode binary formatı
[x] Türkçe assembler (zed_as)
[x] Çoklu modül linker (zed_link)
[x] libZED standart kütüphane
[x] Kernel v1.0
[x] Program yükleme ve çalıştırma (INT #50)
[x] **Gerçek donanım desteği (x86 Bootloader)**
[x] **VGA Grafik ve Metin motoru**
[ ] Türkçe yazılım terimleri sözlüğü
[ ] Dosya sistemi komutları (listele, sil)
[ ] Çoklu görev desteği
[ ] VS Code sözdizimi desteği
[ ] Tam Türkçe işletim sistemi
Neden Zedin Assembly?
Türkiye'de yazılım geliştirmenin önündeki engellerden biri İngilizce terimler. Zedin Assembly, Türkçe düşünen zihinler için tasarlandı — ama altında tam anlamıyla profesyonel bir dil altyapısı var. Assembler'dan linker'a, VM'den kernel'e kadar her şey Türkçe komutlarla çalışıyor.
Sıfırdan. Telefonda. Termux'ta.
