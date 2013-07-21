#ifndef __defs_h__
#define __defs_h__

#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "menu.h"
#include "settings.h"

// Pins
#define PIN_BACKLIGHT    9
#define PIN_PC_TX        10
#define PIN_PC_RX        11
#define PIN_RIG_TX       12
#define PIN_RIG_RX       13

// Activities
#define ACTIVITY_NONE    0x00
#define ACTIVITY_DISPLAY 0x10
#define ACTIVITY_CONFIG  0x20
byte activity = ACTIVITY_DISPLAY;

// Setup LCD
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

// Setup serial
//SoftwareSerial rig(2, 3);

// Setup bluetooth
//SoftwareSerial pc(PIN_PC_RX, PIN_PC_TX);

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
#define DISP_000          0x00
#define DISP_010          0x01
#define DISP_020          0x02
#define DISP_030          0x03
#define DISP_040          0x04
#define DISP_050          0x05
#define DISP_060          0x06
#define DISP_070          0x07
#define DISP_080          0x08
#define DISP_090          0x09
#define DISP_100          0x0a
Setting conf_disp = Setting(0x40, DISP_050);

// Setup menu
void config_select(MenuItemEvent);
void config_change(MenuItemEvent);

Menu menu = Menu(config_change, config_select);
MenuItem menu_conf = MenuItem("Configuration");
MenuItem menu_conf_none = MenuItem("Select option");
MenuItem menu_conf_exit = MenuItem("Exit");
MenuItem menu_conf_nuke = MenuItem("Factory reset");
MenuItem menu_rate = MenuItem("CAT baud rate");
MenuItem menu_rate_4800 = MenuItem("4800 baud");
MenuItem menu_rate_9600 = MenuItem("9600 baud");
MenuItem menu_rate_fast = MenuItem("38400 baud");
MenuItem menu_ping = MenuItem("CAT ping wait");
MenuItem menu_ping_0001 = MenuItem("1 second");
MenuItem menu_ping_0005 = MenuItem("5 seconds");
MenuItem menu_ping_0015 = MenuItem("15 seconds");
MenuItem menu_disp = MenuItem("Display light ");
MenuItem menu_disp_0000 = MenuItem("Backlight  off");
MenuItem menu_disp_0010 = MenuItem("Backlight  10%");
MenuItem menu_disp_0020 = MenuItem("Backlight  20%");
MenuItem menu_disp_0030 = MenuItem("Backlight  30%");
MenuItem menu_disp_0040 = MenuItem("Backlight  40%");
MenuItem menu_disp_0050 = MenuItem("Backlight  50%");
MenuItem menu_disp_0060 = MenuItem("Backlight  60%");
MenuItem menu_disp_0070 = MenuItem("Backlight  70%");
MenuItem menu_disp_0080 = MenuItem("Backlight  80%");
MenuItem menu_disp_0090 = MenuItem("Backlight  90%");
MenuItem menu_disp_0100 = MenuItem("Backlight full");

void setup_menu() {
  //MenuItem temp = NULL;
  // Link menu items
  menu.add(&menu_conf);
  menu_conf.add(&menu_conf_none);
  menu_conf.add(&menu_conf_exit);
  menu_conf.add(&menu_conf_nuke);
  menu.add(&menu_rate);
  menu_rate.add(&menu_rate_4800);
  menu_rate.add(&menu_rate_9600);
  menu_rate.add(&menu_rate_fast);
  menu.add(&menu_ping);
  menu_ping.add(&menu_ping_0001);
  menu_ping.add(&menu_ping_0005);
  menu_ping.add(&menu_ping_0015);
  menu.add(&menu_disp);
  menu_disp.add(&menu_disp_0000);
  menu_disp.add(&menu_disp_0010);
  menu_disp.add(&menu_disp_0020);
  menu_disp.add(&menu_disp_0030);
  menu_disp.add(&menu_disp_0040);
  menu_disp.add(&menu_disp_0050);
  menu_disp.add(&menu_disp_0060);
  menu_disp.add(&menu_disp_0070);
  menu_disp.add(&menu_disp_0080);
  menu_disp.add(&menu_disp_0090);
  menu_disp.add(&menu_disp_0100);
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
  { 0x07, 0x04, 0x06, 0x01, 0x16, 0x00, 0x00, 0x00 }, /* half */
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* free */
  { 0x00, 0x0a, 0x1f, 0x1f, 0x0e, 0x04, 0x00, 0x00 }  /* heart */
};
byte display_bar[6] = { 0x20, 0x01, 0x02, 0x03, 0x04, 0xff };

#endif
