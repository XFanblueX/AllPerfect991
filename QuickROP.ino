#include <Arduino.h>
#include <Wire.h>
#include "Command.hpp"
#include "Key.hpp"

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


void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(clkPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(enablePin, HIGH);
  digitalWrite(latchPin, LOW);

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
    processCommand(inputBuffer);
    inputBuffer = "";
    commandReady = false;
  }
}