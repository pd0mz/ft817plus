#ifndef __ft817_h__
#define __ft817_h__

#include <../SoftwareSerial/SoftwareSerial.h>

// From http://www.ka7oei.com/ft817_meow.html
#define CMD_LOCK_ON          0x00
#define CMD_FREQ_SET         0x01
#define CMD_SPLIT_ON         0x02
#define CMD_FREQ_MODE_GET    0x03
#define CMD_CLAR_ON          0x05
#define CMD_MODE_SET         0x07
#define CMD_PTT_ON           0x08
#define CMD_REPEATER_OFFSET  0x09
#define CMD_CTCSS_SET        0x0a
#define CMD_CTCSS_TONE       0x0b
#define CMD_DCS_CODE_SET     0x0c
#define CMD_ON               0x0f
#define CMD_LOCK_OFF         0x80
#define CMD_VFO_TOGGLE       0x81
#define CMD_SPLIT_OFF        0x82
#define CMD_CLAR_OFF         0x85
#define CMD_OFF              0x8f
#define CMD_TX_METERS        0xbd
#define CMD_RX_STATUS        0xe7
#define CMD_TX_STATUS        0xf7

SoftwareSerial rig = SoftwareSerial(2, 3); // rx, tx

/* prog_uchar RadioSMeter[][6] PROGMEM = { */
const char RadioSMeter[][6] = {
  "S0   ",  // 0x0
  "S1   ",  // 0x1
  "S2   ",  // 0x2
  "S3   ",  // 0x3
  "S4   ",  // 0x4
  "S5   ",  // 0x5
  "S6   ",  // 0x6
  "S7   ",  // 0x7
  "S8   ",  // 0x8
  "S9   ",  // 0x9
  "S9+10",  // 0xa
  "S9+20",  // 0xb
  "S9+30",  // 0xc
  "S9+40",  // 0xd
  "S9+50",  // 0xe
  "S9+60",  // 0xf
};

const char RadioMode[][4] = {
  "LSB",    // 0x0
  "USB",    // 0x1
  "CW ",    // 0x2
  "CWR",    // 0x3
  "AM ",    // 0x4
  "   ",
  "WFM",    // 0x6
  "   ",
  "FM ",    // 0x8
  "   ",
  "DIG",    // 0xa
  "   ",
  "PKT",    // 0xc
  "   ",
  "   ",
  "   "
};

const char RadioPower[][3] = {
    "0\x05",    /* 0.5 Watt */
    "1 ",       /* 1.0 Watt */
    "2\x05",    /* 2.5 Watt */
    "5 "        /* 5.0 Watt */
};

struct FrequencyMode {
  unsigned long frequency;
  byte mode;
};

struct RXStatus {
  byte s_meter;
  boolean centered;
  boolean matched;
  boolean squelched;
};

struct TXMeters {
  byte power;
  byte vswr;
  byte alc;
  byte mod;
};

struct TXStatus {
  byte power;
  boolean split;
  boolean high_swr;
  boolean ptt;
};


class Radio {
public:
  Radio() {
    _timeout = 2000; // 2 seconds
  }
  
  /* High level interface */
  
  FrequencyMode getFrequencyMode() {
    FrequencyMode fm = {0, 0};
    byte sequence[5] = {0x00, 0x00, 0x00, 0x00, CMD_FREQ_MODE_GET};
    byte frequency[4] = {0x00, 0x00, 0x00, 0x00};
    unsigned long start = millis();
    
    flush(); // clear RX buffer
    send(sequence, 5);
    
    // Wait until we have 5 bytes back
    while (Serial.available() < 5) {
      if ((millis() - start) > _timeout) {
        return fm;
      }
    }
    
    // Read 5 bytes
    for (byte i = 0; i < 4; i++) {
      frequency[i] = read();
    }
    fm.frequency = _from_bcd(frequency, 8);
    fm.mode = read();
    return fm;
  }
  
  unsigned long getFrequency() {
    FrequencyMode fm = getFrequencyMode();
    return fm.frequency;
  }
  
  byte getMode() {
    FrequencyMode fm = getFrequencyMode();
    return fm.mode;
  }
  
  RXStatus getRXStatus() {
    byte data = single(CMD_RX_STATUS);
    RXStatus info = {
        (byte) (data & (byte) 0xf),
        (byte) (data >> 5 & 1) == 0,
        (byte) (data >> 6 & 1) == 0,
        (byte) (data >> 7 & 1) == 1
    };
    return info;
  }
  
  TXMeters getTXMeters() {
    byte data = single(CMD_TX_METERS);
    TXMeters info = {0, 0, 0, 0};
    if (data == 0) {
      return info;
    }
    info.power = (data >> 4 & 0xf);
    info.vswr  = (data & 0xf);
    data = read();
    info.alc = (data >> 4 & 0xf);
    info.mod = (data & 0xf);
    return info;
  } 
  
  TXStatus getTXStatus() {
    byte data = single(CMD_TX_STATUS);
    TXStatus info = {
      (byte) (data & (byte) 0xf),
      (byte) (data >> 5 & 1) == 0,
      (byte) (data >> 6 & 1) == 1,
      (byte) (data >> 7 & 1) == 1
    };
    return info;
  }

  boolean transmitting() {
    byte reply = single(CMD_TX_STATUS);
    flush();
    return (reply != 0);
  }

  /* Medium level interface */
  
  byte single(byte cmd) {
    byte sequence[5] = {0x00, 0x00, 0x00, 0x00, cmd};
    send(sequence, 5);
    return read();
  }
  
  /* Low level interface */
  
  byte read() {
    unsigned long start = millis();
    while (Serial.available() < 1) {
      if ((millis() - start) > _timeout) {
        return 0;
      }
    }
    return Serial.read();
  }
      
  void send(byte sequence[], byte length) {
    for (byte i = 0; i < length; i++) {
      Serial.write(sequence[i]);
    }
  }
  
  void flush() {
    Serial.flush();
  }


  
private:
  unsigned int _timeout;
  
  // GPL code, from hamlib
  unsigned long _from_bcd(const byte data[], unsigned int length) {
    unsigned long f = 0;
    for (unsigned int i = 0; i < (length / 2); i++) {
      f *= 10;
      f += data[i] >> 0x04;
      f *= 10;
      f += data[i] & 0x0f;
    }
    if (length & 1) {
      f *= 10;
      f += data[length / 2] >> 0x04;
    }
    return f;
  }
  
  unsigned char* _to_bcd(unsigned char data[], unsigned long f, unsigned int length) {
    unsigned char a;
    if (length & 1) {
      data[length / 2] &= 0x0f;
      data[length / 2] |= (f % 10) << 0x04;
      f /= 10;
    }
    for (unsigned int i = (length / 2) - 1; i >= 0; i--) {
      a = f % 10;
      f /= 10;
      a |= (f % 10) << 4;
      f /= 10;
      data[i] = a;
    }
    return data;
  }
};

#endif
