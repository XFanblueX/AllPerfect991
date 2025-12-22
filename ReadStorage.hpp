#ifndef READSTORAGE_H
#define READSTORAGE_H

#include <Arduino.h>
#include <Wire.h>

byte readStorage(byte deviceAddr, unsigned int memAddr);

#endif