#ifndef __keys_h__
#define __keys_h__

#define ADC_NONE    0x050  // Spurs
#define ADC_0A      0x240
#define ADC_0B      0x210
#define ADC_TOP     0x2e0
#define ADC_1A      0x270
#define ADC_1B      0x290
#define ADC_BOTTOM  0x330

#define KEY_NONE    0x00
#define KEY_0A      0x10
#define KEY_0B      0x11
#define KEY_1A      0x20
#define KEY_1B      0x31
#define KEY_TOP     0xe0
#define KEY_BOTTOM  0xf0

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
            if (adc <= ADC_NONE)    return KEY_NONE;
            if (adc <= ADC_0B)      return KEY_0B;
            if (adc <= ADC_0A)      return KEY_0A;
            if (adc <= ADC_1A)      return KEY_1A;
            if (adc <= ADC_1B)      return KEY_1B;
            if (adc <= ADC_TOP)     return KEY_TOP;
            if (adc <= ADC_BOTTOM)  return KEY_BOTTOM;
            return KEY_NONE;
        }
};

#endif
