#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <Arduino.h>

extern const int dataPin;
extern const int clkPin;
extern const int latchPin;
extern const int enablePin;
extern const int deviceAddress;

extern int keyTime;
extern int keyDelay;
extern const byte keyList[];
extern const int keyListSize;

void processCommand(String fullCommand);
void cmdIsUp(String params);
void cmdDiag(String params);
void cmdKey(String params);
void cmdSet(String params);
void cmdGet(String params);
void cmdRun(String params);
void cmdCheck(String params);

#endif