#include <Arduino.h>
#include <Wire.h>
#include "ReadStorage.hpp"

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

// 命令结构定义
struct Command {
  const char* name;      // 命令名
  void (*func)(String);  // 处理函数
};

struct Option {
  const char* name;
  int* option;
};

void cmdIsUp(String params);
void cmdDiag(String params);
void cmdKey(String params);
void cmdSet(String params);
void cmdGet(String params);
void cmdRun(String params);

Command commands[] = {
  { "ISUP", cmdIsUp },
  { "DIAG", cmdDiag },
  { "KEY", cmdKey },
  { "SET", cmdSet },
  { "GET", cmdGet },
  { "RUN", cmdRun }
};

Option options[] = {
  { "KEYTIME", &keyTime },
  { "KEYDELAY", &keyDelay }
};

const int commandCount = sizeof(commands) / sizeof(Command);
const int optionCount = sizeof(options) / sizeof(Option);

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

void keyStroke(byte keyValue) {
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

// byte readROM(byte deviceAddr, unsigned int memAddr) {
//   byte data = 0xFF;

//   Wire.beginTransmission(deviceAddr);
//   Wire.write((byte)(memAddr >> 8));    // 地址高字节
//   Wire.write((byte)(memAddr & 0xFF));  // 地址低字节
//   Wire.endTransmission();

//   Wire.requestFrom(deviceAddr, (byte)1);
//   if (Wire.available()) {
//     data = Wire.read();
//   }
//   return data;
// }

void processCommand(String fullCommand) {
  fullCommand.trim();
  Serial.print("> ");
  Serial.println(fullCommand);

  int spaceIndex = fullCommand.indexOf(' ');
  String cmdName = fullCommand;
  String params = "";

  if (spaceIndex > 0) {
    cmdName = fullCommand.substring(0, spaceIndex);
    params = fullCommand.substring(spaceIndex + 1);
    params.trim();
  }

  cmdName.toUpperCase();

  // 查找并执行命令
  bool found = false;
  for (int i = 0; i < commandCount; i++) {
    if (cmdName.equals(commands[i].name)) {
      commands[i].func(params);
      found = true;
      break;
    }
  }

  if (!found) {
    Serial.println("Invalid command");
  }
}

void cmdIsUp(String params) {
  Serial.println("OK");
}

void cmdDiag(String params) {
  Serial.print("Running diagnostic...");
  for (int i = 0; i < (sizeof(keyList) / sizeof(keyList[0])); i++) {
    keyStroke(keyList[i]);
  }
  Serial.println("OK");
}

void cmdKey(String params) {

  unsigned int keyValue;
  int count = sscanf(params.c_str(), "%x", &keyValue);

  bool valid = false;
  if (count == 1) {
    for (int i = 0; i < (sizeof(keyList) / sizeof(keyList[0])); i++) {
      if (keyValue == keyList[i])
        valid = true;
    }
  }

  if (valid) {
    keyStroke(keyValue);
    Serial.println("OK");
  } else {
    Serial.println("Invalid key value");
    return;
  }
}

void cmdSet(String params) {

  int spaceIndex = params.indexOf(' ');

  if (spaceIndex <= 0) {
    Serial.println("No enough arguments");
    return;
  }

  String opt = params.substring(0, spaceIndex);
  String value = params.substring(spaceIndex + 1);


  int valueSet = value.toInt();

  opt.toUpperCase();
  bool found = false;

  for (int i = 0; i < optionCount; i++) {
    if (opt.equals(options[i].name)) {
      *options[i].option = valueSet;
      DEBUG_PRINTLN(value);
      DEBUG_PRINTLN(valueSet);
      DEBUG_PRINT(options[i].name);
      found = true;
      break;
    }
  }

  if (!found) {
    Serial.println("Invalid option");
  }
}

void cmdGet(String params) {
  params.toUpperCase();

  for (int i = 0; i < optionCount; i++) {
    if (params.equals(options[i].name)) {
      Serial.println(*options[i].option);
      break;
    }
  }
}

void cmdRun(String params) {
  unsigned int beginAddr, endAddr;
  int count = sscanf(params.c_str(), "%x %x", &beginAddr, &endAddr);
  DEBUG_PRINTLN(count);
  DEBUG_PRINTLN(beginAddr, HEX);
  DEBUG_PRINTLN(endAddr, HEX);
  if (count != 2 || beginAddr > endAddr) {
    Serial.println("Invalid address");
    return;
  }

  for (int addr = beginAddr; addr <= endAddr; addr++){
    DEBUG_PRINT(addr,HEX);
    DEBUG_PRINT(":");
    DEBUG_PRINTLN(readROM(deviceAddress, addr), HEX);
    keyStroke(readROM(deviceAddress, addr));
    Serial.print(((float)(addr - beginAddr) / (float)(endAddr - beginAddr))*100, 2);
    Serial.println("%");
  }
  Serial.println("OK");
}
