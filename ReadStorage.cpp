#include <Arduino.h>
#include "ReadStorage.hpp"

byte readROM(byte deviceAddr, unsigned int memAddr) {
  byte data = 0xFF;

  Wire.beginTransmission(deviceAddr);
  Wire.write((byte)(memAddr >> 8));    // 地址高字节
  Wire.write((byte)(memAddr & 0xFF));  // 地址低字节
  Wire.endTransmission();

  Wire.requestFrom(deviceAddr, (byte)1);
  if (Wire.available()) {
    data = Wire.read();
  }
  return data;
}