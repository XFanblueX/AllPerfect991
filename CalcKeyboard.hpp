#ifndef CALCKEYBOARD_H
#define CALCKEYBOARD_H 

#include <Arduino.h>

extern const byte keyList[];
extern const int keyListSize;

class CalcKeyboard { 
public:
    CalcKeyboard(int data, int clk, int latch, int enable);
    ~CalcKeyboard();
    void press(byte keyValue);
    void runDiag();
    bool checkKeyValue(byte keyValue);

    bool setOption(String opt, int value);
    int getOption(String opt);

private:

    struct Option {
        const char* name;
        int* option;
    };
    int dataPin;
    int clkPin;
    int latchPin;
    int enablePin;
    int keyTime;
    int keyDelay;

    int optionsSize;
    Option *options;

};

#endif