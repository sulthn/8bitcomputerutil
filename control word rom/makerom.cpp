// ROM Programmer for 8-Bit CPU

#include <stdio.h>
#include <string.h>

#define MMO 0x00000001
#define PCO 0x00000002
#define AO  0x00000003
#define EO  0x00000004
#define STO 0x00000005
#define SUD 0x00000006
#define SU  0x00000007

#define CS  0b00000000000000000000000010000000
#define PCE 0b00000000000000000000000001000000
#define SA  0b00000000000000000000000000100000
#define TR  0b00000000000000000000000000010000
#define HLT 0b00000000000000000000000000001000

#define IRI 0x00000100
#define MMI 0x00000200
#define MAI 0x00000300
#define J   0x00000400
#define AI  0x00000500
#define TMP 0x00000600
#define OUT 0x00000700

#define STE 0b00000000000000001000000000000000
#define E1  0b00000000000000000100000000000000
#define E0  0b00000000000000000010000000000000
#define SE  0b00000000000000000001000000000000
#define SH  0b00000000000000000000100000000000

//#define CO  0x00010000
//#define DO  0x00020000
//#define LO  0x00030000
//#define BIC 0x00040000
//#define BDC 0x00050000

//#define CS0 0b00000000010000000000000000000000
//#define CAR 0b00000000001000000000000000000000
#define SEC 0b00000000000100000000000000000000
#define FI  0b00000000000010000000000000000000

//#define CI  0x01000000 // BOT LEFT CON TO LEFT
//#define DI  0x02000000
//#define DSI 0x03000000
//#define ES0 0x04000000
//#define ES1 0x05000000
//#define FS  0b10000000000000000000000000000000
//#define DSE 0b01000000000000000000000000000000
//#define DRS 0b00100000000000000000000000000000
//#define LS0 0b00010000000000000000000000000000
//#define LS1 0b00001000000000000000000000000000

#define FLAGS_N0Z0C0 0
#define FLAGS_N0Z0C1 1
#define FLAGS_N0Z1C0 2
#define FLAGS_N0Z1C1 3
#define FLAGS_N1Z0C0 4
#define FLAGS_N1Z0C1 5
#define FLAGS_N1Z1C0 6
#define FLAGS_N1Z1C1 7

#define JC 0b00000111
#define JZ 0b00001000
#define JN 0b00010001

const unsigned long UCODE_TEMPLATE[256][8] = {
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00000000 - NOP
                           // MAI|PCO,               MAI|MMO|PCE,        MMO|AI,           TR,                 0,      0  },
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MAI|MMO|PCE,        MMO|AI|FI,        TR,                 0,      0  },          // 00000001 - LDA
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MAI|MMO|PCE,        MMO|TMP,          EO|AI|FI,           TR,     0  },          // 00000010 - ADD
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MAI|MMO|PCE,        MMO|TMP,          SU|EO|AI|FI,        TR,     0  },          // 00000011 - SUB
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MAI|MMO|PCE,        MMI|AO,           TR,                 0,      0  },          // 00000100 - STA
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MMO|AI|PCE|FI,      TR,               0,                  0,      0  },          // 00000101 - LDA (Imm)
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MMO|J,              TR,               0,                  0,      0  },          // 00000110 - JMP
    { MAI|PCO,  MMO|IRI|PCE,  PCE,                   TR,                 0,                0,                  0,      0  },          // 00000111 - JC
    { MAI|PCO,  MMO|IRI|PCE,  PCE,                   TR,                 0,                0,                  0,      0  },          // 00001000 - JZ
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MMO|TMP|PCE,        EO|AI|FI,         TR,                 0,      0  },          // 00001001 - ADD (Imm)
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MMO|TMP|PCE,        SU|EO|AI|FI,      TR,                 0,      0  },          // 00001010 - SUB (Imm)
    { MAI|PCO,  MMO|IRI|PCE,  MAI|STO,               AO|SA|MMI|STE|SUD,  TR,               0,                  0,      0  },          // 00001011 - PHA
    { MAI|PCO,  MMO|IRI|PCE,  STE,                   MAI|STO,            MMO|AI|SA,        TR,                 0,      0  },          // 00001100 - PLA
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MAI|MMO|PCE,        MMO|TMP,          EO|FI,              TR,     0  },          // 00001101 - CPA
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MAI|MMO|PCE,        MMO|OUT,          TR,                 0,      0  },          // 00001110 - OUT
    { MAI|PCO,  MMO|IRI|PCE,  HLT,                   0,                  0,                0,                  0,      0  },          // 00001111 - HLT
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00010000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  PCE,                   TR,                 0,                0,                  0,      0  },          // 00010001 - JN
    { MAI|PCO,  MMO|IRI|PCE,  SEC|FI,                TR,                 0,                0,                  0,      0  },          // 00010010 - SEC
    { MAI|PCO,  MMO|IRI|PCE,  FI,                    TR,                 0,                0,                  0,      0  },          // 00010011 - CLC
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00010100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00010101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00010110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00010111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00011000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  MAI|STO,               PCO|SA|MMI|STE|SUD, MAI|PCO,          J|MMO,              TR,     0  },          // 00011001 - JSR
    { MAI|PCO,  MMO|IRI|PCE,  STE,                   MAI|STO,            MMO|J|SA,         TR,                 0,      0  },          // 00011010 - RTS
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00011011 - NOP   
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00011100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00011101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  AO|OUT,                TR,                 0,                0,                  0,      0  },          // 00011110 - OUT (A REG)
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MMO|TMP|PCE,        SU|EO|AI|FI,      TR,                 0,      0  },          // 00011111 - CMP (Imm)
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00100000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00100001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00100010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00100011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00100100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00100101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00100110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  SE|FI,                 TR,                 0,                0,                  0,      0  },          // 00100111 - ASL
    { MAI|PCO,  MMO|IRI|PCE,  SE|SH|FI,              TR,                 0,                0,                  0,      0  },          // 00101000 - LSR
    { MAI|PCO,  MMO|IRI|PCE,  SE|CS|FI,              TR,                 0,                0,                  0,      0  },          // 00101001 - ROL
    { MAI|PCO,  MMO|IRI|PCE,  SE|SH|CS|FI,           TR,                 0,                0,                  0,      0  },          // 00101010 - ROR
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00101011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00101100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00101101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00101110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00101111 - NOP  
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00110000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00110001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00110010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00110011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00110100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00110101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00110110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MAI|MMO|PCE,        MMO|TMP,          E1|EO|AI|FI,        TR,     0  },          // 00110111 - AND
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MAI|MMO|PCE,        MMO|TMP,          E0|EO|AI|FI,        TR,     0  },          // 00111000 - ORA
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MAI|MMO|PCE,        MMO|TMP,          E0|E1|EO|AI|FI,     TR,     0  },          // 00111001 - XOR
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MMO|TMP|PCE,        E1|EO|AI|FI,      TR,                 0,      0  },          // 00111010 - AND (Imm)
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MMO|TMP|PCE,        E0|EO|AI|FI,      TR,                 0,      0  },          // 00111011 - ORA (Imm)
    { MAI|PCO,  MMO|IRI|PCE,  MAI|PCO,               MMO|TMP|PCE,        E0|E1|EO|AI|FI,   TR,                 0,      0  },          // 00111100 - XOR (Imm)
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00111101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00111110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 00111111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01000000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01000001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01000010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01000011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01000100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01000101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01000110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01000111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01001000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01001001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01001010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01001011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01001100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01001101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01001110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01001111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01010000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01010001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01010010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01010011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01010100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01010101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01010110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01010111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01011000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01011001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01011010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01011011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01011100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01011101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01011110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,                    0,                  0,                0,                  0,      0  },          // 01011111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01100000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01100001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01100010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01100011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01100100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01100101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01100110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01100111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01101000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01101001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01101010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01101011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01101100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01101101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01101110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01101111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01110000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01110001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01110010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01110011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01110100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01110101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01110110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01110111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01111000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01111001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01111010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01111011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01111100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01111101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01111110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 01111111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10000000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10000001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10000010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10000011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10000100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10000101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10000110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10000111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10001000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10001001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10001010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10001011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10001100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10001101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10001110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10001111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10010000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10010001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10010010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10010011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10010100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10010101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10010110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10010111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10011000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10011001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10011010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10011011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10011100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10011101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10011110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10011111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10100000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10100001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10100010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10100011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10100100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10100101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10100110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10100111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10101000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10101001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10101010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10101011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10101100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10101101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10101110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10101111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10110000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10110001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10110010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10110011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10110100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10110101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10110110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10110111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10111000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10111001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10111010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10111011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10111100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10111101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10111110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 10111111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11000000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11000001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11000010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11000011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11000100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11000101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11000110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11000111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11001000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11001001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11001010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11001011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11001100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11001101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11001110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11001111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11010000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11010001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11010010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11010011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11010100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11010101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11010110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11010111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11011000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11011001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11011010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11011011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11011100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11011101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11011110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11011111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11100000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11100001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11100010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11100011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11100100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11100101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11100110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11100111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11101000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11101001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11101010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11101011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11101100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11101101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11101110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11101111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11110000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11110001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11110010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11110011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11110100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11110101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11110110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11110111 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11111000 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11111001 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11111010 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11111011 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11111100 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11111101 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11111110 - NOP
    { MAI|PCO,  MMO|IRI|PCE,  TR,        0,            0,            0,           0,   0 },          // 11111111 - NOP
};

unsigned long ucode[8][256][8];

void initUCode() {
  // NF = 0, ZF = 0, CF = 0
  memcpy(ucode[FLAGS_N0Z0C0], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));

  // NF = 0, ZF = 0, CF = 1
  memcpy(ucode[FLAGS_N0Z0C1], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_N0Z0C1][JC][2] = MAI|PCO;
  ucode[FLAGS_N0Z0C1][JC][3] = MMO|J;
  ucode[FLAGS_N0Z0C1][JC][4] = TR;

  // NF = 0, ZF = 1, CF = 0
  memcpy(ucode[FLAGS_N0Z1C0], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_N0Z1C0][JZ][2] = MAI|PCO;
  ucode[FLAGS_N0Z1C0][JZ][3] = MMO|J;
  ucode[FLAGS_N0Z1C0][JZ][4] = TR;

  // NF = 0, ZF = 1, CF = 1
  memcpy(ucode[FLAGS_N0Z1C1], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_N0Z1C1][JC][2] = MAI|PCO;
  ucode[FLAGS_N0Z1C1][JC][3] = MMO|J;
  ucode[FLAGS_N0Z1C1][JC][4] = TR;
  ucode[FLAGS_N0Z1C1][JZ][2] = MAI|PCO;
  ucode[FLAGS_N0Z1C1][JZ][3] = MMO|J;
  ucode[FLAGS_N0Z1C1][JZ][4] = TR;

  // NF = 1, ZF = 0, CF = 0
  memcpy(ucode[FLAGS_N1Z0C0], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_N1Z0C0][JN][2] = MAI|PCO;
  ucode[FLAGS_N1Z0C0][JN][3] = MMO|J;
  ucode[FLAGS_N1Z0C0][JN][4] = TR;

  // NF = 1, ZF = 0, CF = 1
  memcpy(ucode[FLAGS_N1Z0C1], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_N1Z0C1][JN][2] = MAI|PCO;
  ucode[FLAGS_N1Z0C1][JN][3] = MMO|J;
  ucode[FLAGS_N1Z0C1][JN][4] = TR;
  ucode[FLAGS_N1Z0C1][JC][2] = MAI|PCO;
  ucode[FLAGS_N1Z0C1][JC][3] = MMO|J;
  ucode[FLAGS_N1Z0C1][JC][4] = TR;

  // NF = 1, ZF = 1, CF = 0
  memcpy(ucode[FLAGS_N1Z1C0], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_N1Z1C0][JN][2] = MAI|PCO;
  ucode[FLAGS_N1Z1C0][JN][3] = MMO|J;
  ucode[FLAGS_N1Z1C0][JN][4] = TR;
  ucode[FLAGS_N1Z1C0][JZ][2] = MAI|PCO;
  ucode[FLAGS_N1Z1C0][JZ][3] = MMO|J;
  ucode[FLAGS_N1Z1C0][JZ][4] = TR;

  // NF = 1, ZF = 1, CF = 1
  memcpy(ucode[FLAGS_N1Z1C1], UCODE_TEMPLATE, sizeof(UCODE_TEMPLATE));
  ucode[FLAGS_N1Z1C1][JN][2] = MAI|PCO;
  ucode[FLAGS_N1Z1C1][JN][3] = MMO|J;
  ucode[FLAGS_N1Z1C1][JN][4] = TR;
  ucode[FLAGS_N1Z1C1][JC][2] = MAI|PCO;
  ucode[FLAGS_N1Z1C1][JC][3] = MMO|J;
  ucode[FLAGS_N1Z1C1][JC][4] = TR;
  ucode[FLAGS_N1Z1C1][JZ][2] = MAI|PCO;
  ucode[FLAGS_N1Z1C1][JZ][3] = MMO|J;
  ucode[FLAGS_N1Z1C1][JZ][4] = TR;
}

int main (void)
{
  initUCode();

  char ucode_top_upper[65536];
  char ucode_top_lower[65536];
  char ucode_bot_upper[65536];
  char ucode_bot_lower[65536];

  for (int address = 0; address < 65536; address++) {
    int flags = (address & 0b0011100000000000) >> 11;
    int instruction = (address & 0b0000011111111000) >> 3;
    int step =  (address & 0b0000000000000111);

    ucode_top_lower[address] = (ucode[flags][instruction][step]);
    ucode_top_upper[address] = (ucode[flags][instruction][step] >> 8);
    ucode_bot_lower[address] = (ucode[flags][instruction][step] >> 16);
    ucode_bot_upper[address] = (ucode[flags][instruction][step] >> 24);
  }

  FILE *f = fopen("rom_top_upper.hex", "wb");
  fwrite(ucode_top_upper, sizeof(char), sizeof(ucode_top_upper), f);
  fclose(f);

  f = fopen("rom_top_lower.hex", "wb");
  fwrite(ucode_top_lower, sizeof(char), sizeof(ucode_top_lower), f);
  fclose(f);

  f = fopen("rom_bot_lower.hex", "wb");
  fwrite(ucode_bot_lower, sizeof(char), sizeof(ucode_bot_lower), f);
  fclose(f);
  
  f = fopen("rom_bot_upper.hex", "wb");
  fwrite(ucode_bot_upper, sizeof(char), sizeof(ucode_bot_upper), f);
  fclose(f);

  return 0;
}