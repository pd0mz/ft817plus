#ifndef __bookmark_h__
#define __bookmark_h__

#include <avr/pgmspace.h>

struct Bookmark {
    unsigned long   frequency;
    const char*     name;
};

const Bookmark bookmarks[] PROGMEM = {
  {  1838000, "PSK31"},
  {  3580000, "PSK31"},
  {  7035000, "PSK31 ITU-2"},
  {  7040000, "PSK31 ITU-1"},
  { 10142000, "PSK31"},
  { 10148000, "PSKmail"},
  { 14070000, "PSK31"},
  { 14111000, "PA0SON-4"},
  { 18100000, "PSK31"},
  { 21080000, "PSK31"},
  { 24920000, "PSK31"},
  { 28120000, "PSK31"},
  { 50290000, "PSK31"},
  {144800000, "APRS Europe"},
  {145600000, "PI3ALK"},
  {145625000, "PI3UTR"},
  {145662500, "PI3ZDM"},
  {146700000, "PI3EHV"},
  {145725000, "PI3ZAZ"},
  {430062500, "PI2NEN"},
  {430075000, "PI2ZST"},
  {430100000, "PI2EHV"},
  {430175000, "PI2YDN"},
  {430125000, "PI2NOS"},
  {430187500, "PI2AMF"},
  {430200000, "PI2ASD"},
  {430212500, "PI2BOZ"},
  {430237500, "PI2SHB"},
  {430250000, "PI2RGK"},
  {430287500, "PI2FLD"},
  {430325000, "PI2ZAZ"},
  {430375000, "PI2OSS"},
  {438900000, "PI1BSM"},
  {        0,     NULL}  // sentinel
};

const char* get_bookmark(unsigned long frequency) {
    const char* name = NULL;
    struct Bookmark b;
    for (byte i = 0;; i++) {
        memcpy_P((void *) &b, (void *) &bookmarks[i], sizeof(struct Bookmark));
        if (b.frequency == 0) {
            break;
        }
        if (b.frequency == frequency) {
            name = b.name;
            break;
        }
    }
    return name;
}

#endif
