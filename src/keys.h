#ifndef __keys_h__
#define __keys_h__

#define KEY_NONE   0x00
#define KEY_UP     0x01
#define KEY_DOWN   0x02
#define KEY_LEFT   0x03
#define KEY_RIGHT  0x04
#define KEY_SELECT 0x05

#define ADC_RIGHT  30
#define ADC_UP     150
#define ADC_DOWN   360
#define ADC_LEFT   535
#define ADC_SELECT 760
#define ADC_NONE   1000

class Key {
public:
  Key(int p) : pin(p) {
    setDebounceTime(100);
    oldKey = 0xff;
  }
  
  byte getKey() {
    int adc = analogRead(pin);
    byte key = getKeyADC(adc);
    if (key != oldKey) {
      // Debounce
      delay(debounceTime);
      adc = analogRead(pin);
      key = getKeyADC(adc);
      if (key != oldKey) {
        oldKey = key;
      } else {
        return KEY_NONE;
      } 
    }
    return key;
  }
  byte waitKey() {
    char wait = KEY_NONE;
    while ((wait = getKey()) == KEY_NONE);
    return wait;
  }
  void setDebounceTime(unsigned int t) {
    debounceTime = t < 1 ? 1 : t;
  }
  
private:
  int pin;
  unsigned int debounceTime;
  byte oldKey;

  byte getKeyADC(int adc) {
    if (adc >= ADC_NONE)   return KEY_NONE;
    if (adc <= ADC_RIGHT)  return KEY_RIGHT;
    if (adc <= ADC_UP)     return KEY_UP;
    if (adc <= ADC_DOWN)   return KEY_DOWN;
    if (adc <= ADC_LEFT)   return KEY_LEFT;
    if (adc <= ADC_SELECT) return KEY_SELECT;
    return KEY_NONE;
  }
};

#endif
