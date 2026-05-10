# Zedin Türkçe Assembly ve VM

Dünyada ilk Türkçe komut setli sanal makine, assembler ve linker zinciri.

## Nedir?

ZedinVM, tamamen Türkçe komutlarla yazılan bir assembly dili ve bu dili çalıştıran bir sanal makinedir. Uzun vadeli hedef, altyapısı tamamen Türkçe olan bir işletim sistemi geliştirmektir.

## Bileşenler

- **oz-islemci** → Sanal makine (VM)
- **zed_as** → Türkçe assembler (.zed → .bin)
- **zed_link** → Linker (çoklu modül birleştirme)
- **libZED** → Standart kütüphane
- **kernel.zed** → İlk Türkçe OS çekirdeği

## Derleme

gcc -Wall -o oz-islemci oz-islemci.c
gcc -Wall -o zed_as zed_as.c
gcc -Wall -o zed_link zed_link.c

## Kullanım

./zed_as program.zed program.bin
./oz-islemci -calistir program.bin

## Kernel

./zed_as kernel.zed kernel.bin
./oz-islemci -calistir kernel.bin

## Komutlar

YUKLE, SAKLA, GETIR, TOPLA, CIKAR, CARP, BOL,
GIT, EGER_ESITSE, EGER_DEGILSE, CALL, RET,
PUSH, POP, INT, YAZDIR, BITIR ve daha fazlası...

## Lisans

MIT
