#ifndef KEY_HPP
#define KEY_HPP

#include <Arduino.h>

extern const int dataPin;
extern const int clkPin;
extern const int latchPin;
extern const int enablePin;

extern int keyTime;
extern int keyDelay;

extern const byte keyList[];
void keyStroke(byte keyValue);

#endif
