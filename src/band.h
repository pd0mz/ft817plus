#ifndef __band_h__
#define __band_h__

#include <avr/pgmspace.h>

struct Band {
  unsigned long frequency_start;
  unsigned long frequency_stop;
  char symbol;
};

struct Band bands[] PROGMEM = {
  // Novice license, The Netherlands
  {  7050000,   7100000, 'N'}, //  40 meter
  { 14000000,  14250000, 'N'}, //  20 meter
  { 28000000,  29700000, 'N'}, //  10 meter
  {144000000, 146000000, 'N'}, //   2 meter
  {430000000, 436000000, 'N'}, //  70 centimeter
  // Full license, The Netherlands
  {  1810000,   1850000, 'F'}, // 160 meter
  {  3500000,   3800000, 'F'}, //  80 meter
  {  7000000,   7200000, 'F'}, //  40 meter
  { 10100000,  10150000, 'F'}, //  30 meter
  { 14000000,  14350000, 'F'}, //  20 meter
  { 18068000,  18168000, 'F'}, //  17 meter
  { 21000000,  21450000, 'F'}, //  15 meter
  { 24890000,  24990000, 'F'}, //  12 meter
  { 28000000,  29700000, 'F'}, //  10 meter
  { 50000000,  52000000, 'F'}, //   6 meter
  { 70000000,  70500000, 'F'}, //   4 meter
  {144000000, 146000000, 'F'}, //   2 meter
  {430000000, 440000000, 'F'}, //  70 centimeter
  {        0,         0,   0}  // sentinel
};

char get_band_symbol(unsigned long frequency) {
    char symbol = 'X';
    struct Band b;
    for (byte i = 0;; i++) {
        memcpy_P((void *) &b, (void *) &bands[i], sizeof(struct Band));
        if (b.symbol == 0) {
            break;
        }
        if (b.frequency_start <= frequency &&
            b.frequency_stop  >= frequency) {
            symbol = b.symbol;
            break;
        }
    }
    return symbol;
}

#endif
