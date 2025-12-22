#include <Arduino.h>
#include <Wire.h>
// #include "Command.hpp"
// #include "Key.hpp"

#include "CalcKeyboard.hpp"
#include "CommandProcessor.hpp"

#define DEBUG 1

#if DEBUG
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINTF(...)
#endif

String inputBuffer = "";
bool commandReady = false;

const int dataPin = 3;    // DS
const int clkPin = 4;     // SH_CP
const int latchPin = 5;   // ST_CP
const int enablePin = 6;  // 74HC595 OE & 74HC4051 E, active low
// const int resetPin = 7;   // MR, active low

const int deviceAddress = 0x50;

int keyTime = 100;
int keyDelay = 50;

const byte keyList[]{
  0x07, 0x17, 0x27, 0x37, 0x47,
  0x06, 0x16, 0x26, 0x36, 0x46, 0x56,
  0x05, 0x15, 0x25, 0x35, 0x45, 0x55,
  0x04, 0x14, 0x24, 0x34, 0x44, 0x54,
  0x03, 0x13, 0x23, 0x33, 0x43, 0x53,
  0x02, 0x12, 0x22, 0x32, 0x42,
  0x01, 0x11, 0x21, 0x31, 0x41,
  0x00, 0x10, 0x20, 0x30, 0x40,
  0x64, 0x63, 0x62, 0x61, 0x60
};


const int keyListSize = sizeof(keyList) / sizeof(keyList[0]);

CalcKeyboard keyboard(dataPin, clkPin, latchPin, enablePin);
CommandProcessor commandProcessor(keyboard);




void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        commandReady = true;
      }
    } else {
      inputBuffer += c;
    }
  }

  if (commandReady) {
    // processCommand(inputBuffer);
    // DEBUG_PRINTLN(inputBuffer);
    commandProcessor.processCommand(inputBuffer);
    inputBuffer = "";
    commandReady = false;
  }
}