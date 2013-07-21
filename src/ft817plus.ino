#include <Arduino.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

#include "defs.h"
#include "keys.h"
#include "settings.h"
#include "radio.h"
#include "band.h"
#include "bookmark.h"

Key keys(A0);
Radio radio;
unsigned int splash = 0;

struct Status {
    /* Activity */
    byte          activity;

    /* Rig settings */
    byte          rig_rate;
    byte          rig_ping;

    /* Rig readings */
    FrequencyMode fm;
    RXStatus      rx;
    TXStatus      tx;
    TXMeters      tx_meters;
    boolean       transmitting;
    byte          power;
    byte          alc;
    byte          mod;

    /* Keys */
    byte          key;
};

Status current = {ACTIVITY_DISPLAY};
Status last = {ACTIVITY_DISPLAY};

void display_begin();

void setup() {
    Serial.begin(38400);
    Serial.println("setup");
    Serial.println(get_bookmark(145600000));

    // Setup LCD display
    pinMode(PIN_BACKLIGHT, OUTPUT);
    analogWrite(PIN_BACKLIGHT, conf_disp.get() * 25);

    lcd.begin(16, 2);
    display_begin();

    // Setup serial to rig
    Serial.println("setup rig cat");
    rig.begin(38400);

    // Setup defaults
    current.rig_rate = conf_rate.get();
    current.rig_ping = conf_ping.get();

    // Setup menu
    Serial.println("setup menu");
    setup_menu();

    // Set default activity
    current.activity = ACTIVITY_DISPLAY;
    last.activity = ACTIVITY_NONE;

    // Set keys
    current.key = KEY_NONE;
    last.key = KEY_NONE;

    // Setup done
    Serial.println("setup done");
}

void config_update() {
    MenuItem* sub = menu.getActive();
    MenuItem* opt = sub->getActive();
    lcd.clear();
    lcd.setCursor(0x0, 0);
    lcd.write(2);
    lcd.print(sub->getName());
    lcd.setCursor(0xf, 0);
    lcd.write(3);
    lcd.setCursor(0x0, 1);
    lcd.write(2);
    lcd.print(opt->getName());
    lcd.setCursor(0xf, 1);
    lcd.write(3);
}

void config_begin() {
    // Setup custom characters
    for (byte i = 0; i < 4; i++) {
        lcd.createChar(i, config_char[i]);
    }

    // Load configured values
    menu_rate.activate(current.rig_rate);
    menu_ping.activate(current.rig_ping);

    // Update display
    config_update();
}

void config_change(MenuItemEvent event) {
    //Serial.println(event.item.getName());
    /* Switching section */
    if (!strcmp(event.item.getName(), "Configuration")) {
        // Default to "exit"
        //menu.activate((&event.item)->getFirst());
    }
    if (!strcmp(event.item.getName(), "CAT baud rate")) {

    }

    /* Switching option */
    if (!strcmp(event.item.getName(), "4800 baud")) {
        conf_rate.set(RIG_RATE_4800);
        rig.begin(4800);
    }
    if (!strcmp(event.item.getName(), "9600 baud")) {
        conf_rate.set(RIG_RATE_9600);
        rig.begin(9600);
    }
    if (!strcmp(event.item.getName(), "38400 baud")) {
        conf_rate.set(RIG_RATE_38400);
        rig.begin(38400);
    }
    if (!strcmp(event.item.getName(), "1 second")) {
        conf_ping.set(RIG_PING_1);
        current.rig_ping = 1;
    }
    if (!strcmp(event.item.getName(), "5 seconds")) {
        conf_ping.set(RIG_PING_5);
        current.rig_ping = 5;
    }
    if (!strcmp(event.item.getName(), "15 seconds")) {
        conf_ping.set(RIG_PING_15);
        current.rig_ping = 15;
    }
    if (!strcmp(event.item.getName(), "Backlight  off")) {
        conf_disp.set(DISP_000);
        analogWrite(PIN_BACKLIGHT, 0);
    }
    if (!strcmp(event.item.getName(), "Backlight  10%")) {
        conf_disp.set(DISP_010);
        analogWrite(PIN_BACKLIGHT, 25);
    }
    if (!strcmp(event.item.getName(), "Backlight  20%")) {
        conf_disp.set(DISP_020);
        analogWrite(PIN_BACKLIGHT, 50);
    }
    if (!strcmp(event.item.getName(), "Backlight  30%")) {
        conf_disp.set(DISP_030);
        analogWrite(PIN_BACKLIGHT, 75);
    }
    if (!strcmp(event.item.getName(), "Backlight  40%")) {
        conf_disp.set(DISP_040);
        analogWrite(PIN_BACKLIGHT, 100);
    }
    if (!strcmp(event.item.getName(), "Backlight  50%")) {
        conf_disp.set(DISP_050);
        analogWrite(PIN_BACKLIGHT, 125);
    }
    if (!strcmp(event.item.getName(), "Backlight  60%")) {
        conf_disp.set(DISP_060);
        analogWrite(PIN_BACKLIGHT, 150);
    }
    if (!strcmp(event.item.getName(), "Backlight  70%")) {
        conf_disp.set(DISP_070);
        analogWrite(PIN_BACKLIGHT, 175);
    }
    if (!strcmp(event.item.getName(), "Backlight  80%")) {
        conf_disp.set(DISP_080);
        analogWrite(PIN_BACKLIGHT, 200);
    }
    if (!strcmp(event.item.getName(), "Backlight  90%")) {
        conf_disp.set(DISP_090);
        analogWrite(PIN_BACKLIGHT, 225);
    }
    if (!strcmp(event.item.getName(), "Backlight full")) {
        conf_disp.set(DISP_100);
        analogWrite(PIN_BACKLIGHT, 250);
    }
    config_update();
}

void config_select(MenuItemEvent event) {
    if (!strcmp(event.item.getName(), "Exit")) {
        display_begin();
        current.activity = ACTIVITY_DISPLAY;
    }
    if (!strcmp(event.item.getName(), "Factory reset")) {
        settings_reset();
        delay(1000);
        setup();
        //    menu.activate(event.item.getFirst());
    }
}

void config_loop() {
}

void display_spam() {
    lcd.setCursor(0, 0);
    lcd.print(" PD0MZ FT-817+ ");
    lcd.write(0x07);
}

void display_scale(byte reading, byte length) {
    for (byte i = 0; i < (reading / 2); i++) {
        lcd.write(0xff);
    }
    if (reading & 1) {
        lcd.write(0x02);
        reading++;
    }
    for (byte i = (reading / 2); i < length; i++) {
        lcd.write(0x20);
    }

}

void display_begin() {
    // Setup custom characters
    for (byte i = 0; i < 8; i++) {
        lcd.createChar(i, display_char[i]);
    }
}

void display_frequency_mode(FrequencyMode fm) {
    lcd.setCursor(0, 0);
    lcd.print(RadioMode[fm.mode]);
    lcd.print(" ");

    // See if we have a memory location for this frequency, if so, print the name
    // rather than the numeric frequency
    const char* name = get_bookmark(fm.frequency);
    if (name != NULL) {
        lcd.print(name);
        lcd.print("     ");
    } else {
        unsigned int Hz  = (fm.frequency % 1000);
        unsigned int kHz = (fm.frequency / 1000) % 1000;
        unsigned int MHz = (fm.frequency / 1000000) % 1000;
        if (MHz < 100) {
            lcd.print(" ");
        }
        if (MHz < 10) {
            lcd.print(" ");
        }
        lcd.print(MHz);
        lcd.print(".");    
        if (kHz < 100) {
            lcd.print("0");
        }
        if (kHz < 10) {
            lcd.print("0");
        }
        lcd.print(kHz);
        lcd.print(".");
        if (Hz < 100) {
            lcd.print("0");
        }
        if (Hz < 10) {
            lcd.print("0");
        }
        lcd.print(Hz);
    }
    lcd.setCursor(15, 0);
    lcd.write(get_band_symbol(fm.frequency));
}

void display_power() {
    lcd.setCursor(0, 0);
    lcd.print("ALC ");
    display_scale(last.tx_meters.alc, 4);
    lcd.setCursor(8, 0);
    lcd.print("MOD ");
    display_scale(last.tx_meters.mod, 4);
    lcd.setCursor(0, 1);
    lcd.print("SWR ");
    switch (last.tx_meters.vswr) {
        case 0x0: lcd.print("1.0"); break;
        case 0x1: lcd.print("1.1"); break;
        case 0x2: lcd.print("1.2"); break;
        case 0x3: lcd.print("1.3"); break;
        case 0x4: lcd.print("1.4"); break;
        case 0x5: lcd.print("1.5"); break;
        case 0x6: lcd.print("1.6"); break;
        case 0x7: lcd.print("1.7"); break;
        case 0x8: lcd.print("1.8"); break;
        case 0x9: lcd.print("1.9"); break;
        case 0xa: lcd.print("2.0"); break;
        case 0xb: lcd.print("2.1"); break;
        case 0xc: lcd.print("2.2"); break;
        case 0xd: lcd.print("2.3"); break;
        case 0xe: lcd.print("2.4"); break;
        case 0xf: lcd.print("HI!"); break;
    }
    lcd.print(" POWER ");
    lcd.print(RadioPower[last.tx_meters.power]);
}

void display_smeter() {
    lcd.setCursor(0, 1);
    lcd.print(RadioSMeter[last.rx.s_meter]);
    lcd.print("           ");
    lcd.setCursor(8, 1);
    display_scale(last.rx.s_meter, 8);
}

byte          tick           = 0;

void display_loop() {
    if (last.transmitting) {
        display_power();
    } else {
        if (last.fm.frequency == 0) {
            display_spam();
            lcd.setCursor(0, 1);
            //         0123456789ABCDEF
            lcd.print("Lost rig contact");
        } else {
            // Update the readings, but only if there is a change
            if (current.rx.s_meter != last.rx.s_meter ||
                current.fm.frequency != last.fm.frequency ||
                current.fm.mode != last.fm.mode) {
                display_smeter();
                display_frequency_mode(last.fm);
                current.rx = last.rx;
                current.fm.frequency = last.fm.frequency;
                current.fm.mode = last.fm.mode;
            }
        }
    }

    // Give rig some slack
    last.transmitting = (last.tx_meters.power != 0xff);
    if (current.transmitting != last.transmitting ||
        (tick++ % 8) == 0) {
        last.fm = radio.getFrequencyMode();
        last.tx = radio.getTXStatus();
        last.rx = radio.getRXStatus();
        last.tx_meters = radio.getTXMeters();
        current.transmitting = last.transmitting;
    }
    delay(50);
}

void loop() {
    switch (current.activity) {
        case ACTIVITY_DISPLAY:
            display_loop();
            break;
        case ACTIVITY_CONFIG:
            config_loop();
            break;
    }

    // Check for keyboard input
    current.key = keys.getKey();
    if (last.key == current.key) {
        return;
    }

    last.key = current.key;
    Serial.print("new key ");
    Serial.println(last.key, HEX);
    switch (current.activity) {
        case ACTIVITY_DISPLAY:
            switch (current.key) {
                case KEY_BOTTOM:
                    current.activity = ACTIVITY_CONFIG;
                    config_begin();
                    break;
            }
            break;

        case ACTIVITY_CONFIG:
            switch (current.key) {
                case KEY_0A:
                    menu.prev();
                    break;
                case KEY_0B:
                    menu.next();
                    break;
                case KEY_1A:
                    menu.getActive()->prev();
                    break;
                case KEY_1B:
                    menu.getActive()->next();
                    break;
                case KEY_BOTTOM:
                    menu.select();
                    break;
            }
            break;
    }
}

