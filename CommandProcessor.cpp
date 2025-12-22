#include "CommandProcessor.hpp"
#include "ReadStorage.hpp"
#include "CalcKeyboard.hpp"
#include <Arduino.h>
#include <Wire.h>

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

CommandProcessor::CommandProcessor(CalcKeyboard &kb) : keyboard(kb) {

    commandCount = 7;
    commands = new Command[commandCount] {
        { "ISUP", &CommandProcessor::cmdIsUp },
        { "DIAG", &CommandProcessor::cmdDiag },
        { "KEY", &CommandProcessor::cmdKey },
        { "SET", &CommandProcessor::cmdSet },
        { "GET", &CommandProcessor::cmdGet },
        { "RUN", &CommandProcessor::cmdRun },
        { "CHECK", &CommandProcessor::cmdCheck }
    };
    Serial.begin(9600);
}

CommandProcessor::~CommandProcessor() {
  delete[] commands;
}

void CommandProcessor::processCommand(String fullCommand) {
  fullCommand.trim();
  DEBUG_PRINTLN("Starting command processor...");
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

  DEBUG_PRINTLN(cmdName);

  // 查找并执行命令
  bool found = false;

  for (int i = 0; i < commandCount; i++) {
    DEBUG_PRINT(i);
    DEBUG_PRINTLN(commands[i].name);
    if (cmdName.equals(commands[i].name)) {
      (this->*commands[i].func)(params);
      found = true;
      break;
    }
  }

  if (!found) {
    Serial.println("Invalid command");
  }
}

void CommandProcessor::cmdIsUp(String params) {
  Serial.println("OK");
}

void CommandProcessor::cmdDiag(String params) {
  Serial.print("Running diagnostic...");
  keyboard.runDiag();
  Serial.println("OK");
}

void CommandProcessor::cmdKey(String params) {
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
    keyboard.press(keyValue);
    Serial.println("OK");
  } else {
    Serial.println("Invalid key value");
    return;
  }
}

void CommandProcessor::cmdSet(String params) {
  int spaceIndex = params.indexOf(' ');

  if (spaceIndex <= 0) {
    Serial.println("No enough arguments");
    return;
  }

  String opt = params.substring(0, spaceIndex);
  String value = params.substring(spaceIndex + 1);

  int valueSet = value.toInt();

  if (keyboard.setOption(opt, valueSet)) {
    Serial.println("OK");
  } else {
    Serial.println("Invalid option");
  }
}

void CommandProcessor::cmdGet(String params) {
  params.toUpperCase();

  Serial.println(keyboard.getOption(params));
}

void CommandProcessor::cmdRun(String params) {
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
    keyboard.press(readStorage(deviceAddress, addr));
    Serial.print(((float)(addr - beginAddr) / (float)(endAddr - beginAddr)) * 100, 2);
    Serial.println("%");
  }
  Serial.println("OK");
}

void CommandProcessor::cmdCheck(String params) {
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

    digitalWrite(LED_BUILTIN, LOW);

    if (!keyboard.checkKeyValue(value)) {
      Serial.print("Invalid key value found at ");
      Serial.println(addr, HEX);
      Serial.print("Value: ");
      Serial.println(value, HEX);
      allValid = false;
      break;
    }
  }

  //   if (!isValid) {
  //     Serial.print("Invalid key value found at ");
  //     Serial.println(addr, HEX);
  //     Serial.print("Value: ");
  //     Serial.println(value, HEX);
  //     allValid = false;
  //     break;
  //   }
  // }

  // if (allValid) {
  Serial.println("OK");
  // }
}