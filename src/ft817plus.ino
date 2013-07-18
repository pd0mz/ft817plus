#include <Arduino.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

#include "defs.h"
#include "keys.h"
#include "settings.h"
#include "radio.h"

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
};

Status current = {ACTIVITY_DISPLAY};
Status last = {ACTIVITY_DISPLAY};

void display_begin();

void setup() {
    // Setup serial to pc
    Serial.begin(38400);
    //Serial.println("setup");

    // Setup LCD display
    lcd.begin(16, 2);
    display_begin();

    // Setup serial to rig
    rig.begin(38400);

    // Setup defaults
    current.rig_rate = conf_rate.get();
    current.rig_ping = conf_ping.get();

    // Setup menu
    setup_menu();

    // Set default activity
    activity = ACTIVITY_DISPLAY;
}

void config_update() {
    MenuItem* sub = menu.getActive();
    MenuItem* opt = sub->getActive();
    lcd.clear();
    lcd.setCursor(0x0, 0);
    lcd.write((uint8_t) 0);
    lcd.print(sub->getName());
    lcd.setCursor(0xf, 0);
    lcd.write(1);
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
    byte key = keys.getKey();
    switch (key) {
        case KEY_UP:
            menu.prev();
            break;
        case KEY_DOWN:
            menu.next();
            break;
        case KEY_LEFT:
            menu.getActive()->prev();
            break;
        case KEY_RIGHT:
            menu.getActive()->next();
            break;
        case KEY_SELECT:
            menu.select();
            break;
    }
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
    boolean label = false;
    for (byte i = 0; repeaters[i].frequency > 0; i++) {
        if (repeaters[i].frequency == fm.frequency) {
            lcd.print(repeaters[i].callsign);
            lcd.print("     ");
            label = true;
        }
    }
    if (!label) {
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
    for (byte i = 0; bands[i].symbol != 0; i++) {
        if (fm.frequency >= bands[i].frequency_start &&
                fm.frequency <= bands[i].frequency_stop) {
            lcd.write(bands[i].symbol);
            return;
        }
    }
    lcd.write('X');
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

unsigned long fake_frequency = 0;
byte          fake_s_meter   = 0;
byte          fake_mode      = 8;
boolean       fake_transmit  = false;
byte          fake_power     = 5;
byte          tick           = 0;

void display_loop() {
    byte key;
    if (splash < (6 * 16)) {
        if (splash == 0) {
            display_begin();
        }
        splash++;
        display_spam();
        int l = splash;
        int b = l % 6;
        int x = (l - b) / 6;
        lcd.setCursor(x, 1);
        lcd.write(display_bar[b]);
    }
    else if (splash == (6 * 16)) {
        splash++;
        lcd.setCursor(0, 0);
        //lcd.print("RX 145.662.500-N");
        lcd.print("Contacting rig..");
        lcd.setCursor(0, 1);
    }
    else {
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
        if ((tick++ % 8) == 0) {
            last.fm = radio.getFrequencyMode();
            last.tx = radio.getTXStatus();
            last.rx = radio.getRXStatus();
            last.tx_meters = radio.getTXMeters();
            last.transmitting = (last.tx_meters.power != 0 ||
                                 last.tx_meters.vswr  != 0);
        }
        delay(10);

        // Check for keyboard input
        key = keys.getKey();
        switch (key) {
            case KEY_SELECT:
                current.activity = ACTIVITY_CONFIG;
                config_begin();
                break;
        }
    }
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
}

