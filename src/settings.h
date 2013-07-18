#ifndef __settings_h__
#define __settings_h__

#include <EEPROM.h>
#define SETTING_UNDEF 0xff

void settings_reset() {
  for (byte i = 0; i < 0xff; i++) {
    EEPROM.write(i, SETTING_UNDEF);
  }
}

class Setting {
public:
  Setting(byte slot, byte defaultValue)
    : _slot(slot) {
    
    _value = load();
    if (_value == SETTING_UNDEF) {
      set(defaultValue);
    }
  }
  
  void set(byte value) {
    EEPROM.write(_slot, value);
    _value = value;
  }
  byte get() {
    return _value;
  }
  byte load() {
    return EEPROM.read(_slot);
  }
  
private:
  byte   _slot;
  byte   _value;
};

#endif
