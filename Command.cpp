#include "Command.hpp"
#include "ReadStorage.hpp"
#include "Key.hpp"
#include <Arduino.h>
#include <Wire.h>

#define DEBUG 0

#if DEBUG
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

struct Command {
  const char* name;
  void (*func)(String);
};

struct Option {
  const char* name;
  int* option;
};

Command commands[] = {
  { "ISUP", cmdIsUp },
  { "DIAG", cmdDiag },
  { "KEY", cmdKey },
  { "SET", cmdSet },
  { "GET", cmdGet },
  { "RUN", cmdRun },
  { "CHECK", cmdCheck }
};

Option options[] = {
  { "KEYTIME", &keyTime },
  { "KEYDELAY", &keyDelay }
};

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
  for (int i = 0; i < sizeof(commands) / sizeof(Command); i++) {
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
  for (int i = 0; i < keyListSize; i++) {
    keyStroke(keyList[i]);
  }
  Serial.println("OK");
}

void cmdKey(String params) {
  unsigned int keyValue;
  int count = sscanf(params.c_str(), "%x", &keyValue);

  bool valid = false;
  if (count == 1) {
    for (int i = 0; i < keyListSize; i++) {
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

  for (int i = 0; i < sizeof(options) / sizeof(Option); i++) {
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

  for (int i = 0; i < sizeof(options) / sizeof(Option); i++) {
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

  for (int addr = beginAddr; addr <= endAddr; addr++) {
    DEBUG_PRINT(addr, HEX);
    DEBUG_PRINT(":");
    DEBUG_PRINTLN(readStorage(deviceAddress, addr), HEX);
    keyStroke(readStorage(deviceAddress, addr));
    Serial.print(((float)(addr - beginAddr) / (float)(endAddr - beginAddr)) * 100, 2);
    Serial.println("%");
  }
  Serial.println("OK");
}

void cmdCheck(String params) {
  unsigned int beginAddr, endAddr;
  int count = sscanf(params.c_str(), "%x %x", &beginAddr, &endAddr);

  if (count != 2 || beginAddr > endAddr) {
    Serial.println("Invalid address");
    return;
  }

  bool allValid = true;
  for (unsigned int addr = beginAddr; addr <= endAddr; addr++) {

    digitalWrite(LED_BUILTIN, HIGH);

    byte value = readStorage(deviceAddress, addr);

    Serial.print(((float)(addr - beginAddr) / (float)(endAddr - beginAddr)) * 100, 2);
    Serial.println("%");

    bool isValid = false;
    for (int i = 0; i < keyListSize; i++) {
      DEBUG_PRINT(value, HEX);
      DEBUG_PRINT(" ");
      DEBUG_PRINT(keyList[i], HEX);
      DEBUG_PRINTLN();
      if (value == keyList[i]) {
        isValid = true;
        break;
      }
    }

    digitalWrite(LED_BUILTIN, LOW);

    if (!isValid) {
      Serial.print("Invalid key value found at ");
      Serial.println(addr, HEX);
      Serial.print("Value: ");
      Serial.println(value, HEX);
      allValid = false;
      break;
    }
  }

  if (allValid) {
    Serial.println("OK");
  }
}