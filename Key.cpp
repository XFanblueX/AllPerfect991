#include "Key.hpp"

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