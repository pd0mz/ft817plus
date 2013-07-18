#ifndef __defs_h__
#define __defs_h__

#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "menu.h"
#include "settings.h"

// Activities
#define ACTIVITY_DISPLAY 0x00
#define ACTIVITY_CONFIG  0x01
byte activity = ACTIVITY_DISPLAY;

// Setup LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Setup serial
//SoftwareSerial rig(2, 3);

// Setup settings
#define RIG_RATE_4800     0x00
#define RIG_RATE_9600     0x01
#define RIG_RATE_38400    0x02
Setting conf_rate = Setting(0x10, RIG_RATE_38400);
#define RIG_PING_OFF      0x00
#define RIG_PING_1        0x01
#define RIG_PING_5        0x02
#define RIG_PING_15       0x03
Setting conf_ping = Setting(0x11, RIG_PING_5);

// Setup menu
void config_select(MenuItemEvent);
void config_change(MenuItemEvent);

Menu menu = Menu(config_change, config_select);
MenuItem menu_conf = MenuItem("Configuration");
MenuItem menu_conf_none = MenuItem("Select option");
MenuItem menu_conf_exit = MenuItem("Exit");
MenuItem menu_conf_save = MenuItem("Save");
MenuItem menu_conf_nuke = MenuItem("Factory reset");
MenuItem menu_rate = MenuItem("CAT baud rate");
MenuItem menu_rate_4800 = MenuItem("4800 baud");
MenuItem menu_rate_9600 = MenuItem("9600 baud");
MenuItem menu_rate_fast = MenuItem("38400 baud");
MenuItem menu_ping = MenuItem("CAT ping wait");
MenuItem menu_ping_0001 = MenuItem("1 second");
MenuItem menu_ping_0005 = MenuItem("5 seconds");
MenuItem menu_ping_0015 = MenuItem("15 seconds");

void setup_menu() {
  //MenuItem temp = NULL;
  // Link menu items
  menu.add(&menu_conf);
  menu_conf.add(&menu_conf_none);
  menu_conf.add(&menu_conf_exit);
  menu_conf.add(&menu_conf_save);
  menu_conf.add(&menu_conf_nuke);
  menu.add(&menu_rate);
  menu_rate.add(&menu_rate_4800);
  menu_rate.add(&menu_rate_9600);
  menu_rate.add(&menu_rate_fast);
  menu.add(&menu_ping);
  menu_ping.add(&menu_ping_0001);
  menu_ping.add(&menu_ping_0005);
  menu_ping.add(&menu_ping_0015);
}

// Additional character maps
byte config_char[4][8] = {
  { 0x00, 0x04, 0x0e, 0x15, 0x04, 0x00, 0x00 },
  { 0x00, 0x04, 0x15, 0x0e, 0x04, 0x00, 0x00 },
  { 0x00, 0x04, 0x08, 0x1f, 0x08, 0x04, 0x00 },
  { 0x00, 0x04, 0x02, 0x1f, 0x02, 0x04, 0x00 }
};
/* Vertically stacked */
/*
byte display_char[8][8] = {

  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f },
  { 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f },
  { 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x1f },
  { 0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f },
  { 0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f },
  { 0x00, 0x0a, 0x1f, 0x1f, 0x0e, 0x04, 0x00 }
};
byte display_bar[8] = { 0x20, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0xff };
*/
/* Horizontally stacked */
byte display_char[8][9] = {
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* free */
  { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 }, /* bar1 */
  { 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18 }, /* bar2 */
  { 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c }, /* bar3 */
  { 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e }, /* bar4 */
  { 0x07, 0x04, 0x07, 0x01, 0x17, 0x00, 0x00, 0x00 }, /* half */
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* free */
  { 0x00, 0x0a, 0x1f, 0x1f, 0x0e, 0x04, 0x00, 0x00 }  /* heart */
};
byte display_bar[6] = { 0x20, 0x01, 0x02, 0x03, 0x04, 0xff };

// Repeater map (for the Netherlands, adjust as desired)
struct Repeater {
  unsigned long frequency;
  const char*   callsign;
};

const Repeater repeaters[] = {
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

struct Band {
  unsigned long frequency_start;
  unsigned long frequency_stop;
  unsigned char symbol;
};

const Band bands[] = {
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

#endif
