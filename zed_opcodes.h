#ifndef ZED_OPCODES_H
enum {
    DUR=0x00, YAZDIR=0x01, TOPLA=0x02,
    SAKLA=0x04, GETIR=0x05,
    EGER_ESITSE=0x06, BUYUKSE=0x07, KUCUKSE=0x08,
    CIKAR=0x09, CARP=0x0A, BOL=0x0B, GIT=0x0C, SAKLA_CEBI=0x0D,
    PUSH=0x0E, POP=0x10, CALL=0x11, RET=0x12, OKU=0x13, KESME=0x14,
    SAKLA_IND=0x15, GETIR_IND=0x16, SAKLA_B=0x17, GETIR_B=0x18,
    EGER_DEGILSE=0x19, VE=0x1A, VEYA=0x1B, MOD=0x1C, DEGIL=0x1D, YUKLE=0x1E
};
#define ZED_OPCODES_H

/* ── ZED Opcode Tanımları ── */
#define ZED_MAGIC 0x5A454400

typedef struct { const char *isim; int op; int boyut; } ZedKomut;

static ZedKomut ZED_KOMUT_SETI[] = {
    {"BITIR",        0x00, 1}, {"YAZDIR",      0x01, 1},
    {"POP",          0x10, 1}, {"RET",         0x12, 1},
    {"DEGIL",        0x1D, 1}, {"TOPLA",       0x02, 2},
    {"GETIR",        0x05, 2}, {"CIKAR",       0x09, 2},
    {"CARP",         0x0A, 2}, {"BOL",         0x0B, 2},
    {"GIT",          0x0C, 2}, {"SAKLA_CEBI",  0x0D, 2},
    {"PUSH",         0x0E, 2}, {"CALL",        0x11, 2},
    {"OKU",          0x13, 2}, {"INT",         0x14, 2},
    {"YUKLE",        0x1E, 2}, {"VE",          0x1A, 2},
    {"VEYA",         0x1B, 2}, {"MOD",         0x1C, 2},
    {"SAKLA",        0x04, 3}, {"EGER_ESITSE", 0x06, 3},
    {"BUYUKSE",      0x07, 3}, {"KUCUKSE",     0x08, 3},
    {"EGER_DEGILSE", 0x19, 3}, {"SAKLA_IND",   0x15, 3},
    {"GETIR_IND",    0x16, 3}, {"SAKLA_B",     0x17, 3},
    {"GETIR_B",      0x18, 3}
};

#define ZED_KOMUT_SAYISI \
    (int)(sizeof(ZED_KOMUT_SETI) / sizeof(ZedKomut))

#endif
