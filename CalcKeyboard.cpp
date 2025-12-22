#include "CalcKeyboard.hpp"
#include <Arduino.h>

#define DEBUG 0

#if DEBUG
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINTF(...)
#endif

CalcKeyboard::CalcKeyboard(int data, int clk, int latch, int enable) {
    optionsSize = 2;
    options = new Option[optionsSize] {
        { "KEYTIME", &keyTime },
        { "KEYDELAY", &keyDelay }
    };
    dataPin = data;
    clkPin = clk;
    latchPin = latch;
    enablePin = enable;

    pinMode(dataPin, OUTPUT);
    pinMode(clkPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(enablePin, OUTPUT);

    digitalWrite(enablePin, HIGH);
    digitalWrite(latchPin, LOW);
}

CalcKeyboard::~CalcKeyboard(){
    delete[] options;
}

void CalcKeyboard::press(byte keyValue) {

    byte highNibble = (keyValue >> 4) & 0x0F;
    byte lowNibble = keyValue & 0x0F;
    byte outputValue = (highNibble << 3) + lowNibble;

    shiftOut(dataPin, clkPin, MSBFIRST, outputValue);

    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(latchPin, HIGH);
    digitalWrite(latchPin, LOW);
    digitalWrite(enablePin, LOW);
    delay(keyTime);
    digitalWrite(enablePin, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
    delay(keyDelay);
}

void CalcKeyboard::runDiag() { 
    for (int i = 0; i < keyListSize; i++) {
        press(keyList[i]);
    }
}

bool CalcKeyboard::checkKeyValue(byte keyValue) {
    for (int i = 0; i < keyListSize; i++) {
      DEBUG_PRINT(keyValue, HEX);
      DEBUG_PRINT(" ");
      DEBUG_PRINT(keyList[i], HEX);
      DEBUG_PRINTLN();
      if (keyValue == keyList[i]) {
        return true;
      }
    }
    return false;

}

bool CalcKeyboard::setOption(String opt, int value) {
    opt.toUpperCase();
    DEBUG_PRINTLN(opt);
    DEBUG_PRINTLN(value);
    for (int i = 0; i < optionsSize; i++) {
        DEBUG_PRINTLN(i);
        DEBUG_PRINTLN(options[i].name);
        if (opt.equals(options[i].name)) {
            *options[i].option = value;
            return true;
        }
    }
    return false;
}

int CalcKeyboard::getOption(String opt) {
    opt.toUpperCase();
    for (int i = 0; i < optionsSize; i++) {
        if (opt.equals(options[i].name)) {
            return *options[i].option;
        }
    }
    return -1;
}