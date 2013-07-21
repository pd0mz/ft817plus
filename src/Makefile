BOARD_TAG	= atmega328
ARDUINO_LIBS	= EEPROM LiquidCrystal SoftwareSerial
MONITOR_PORT	= /dev/ttyUSB*

include ../arduino-mk/arduino-mk/Arduino.mk

test: upload emulate

emulate:
	python2 ../bin/ft817-emulator

debug: upload serial

serial:
	cu -l /dev/ttyUSB0 -s 38400
