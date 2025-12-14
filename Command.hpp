#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <Arduino.h>

extern int keyTime;
extern int keyDelay;
extern const byte keyList[];
extern const int deviceAddress;

void processCommand(String fullCommand);
void cmdIsUp(String params);
void cmdDiag(String params);
void cmdKey(String params);
void cmdSet(String params);
void cmdGet(String params);
void cmdRun(String params);

#endif