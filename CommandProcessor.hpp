#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H 

#include "CalcKeyboard.hpp"
#include <Arduino.h>

extern const int deviceAddress;

class CommandProcessor {
public:
    CommandProcessor(CalcKeyboard &kb);
    ~CommandProcessor();
    void processCommand(String fullCommand);

private:
    struct Command {
        const char* name;
        void (CommandProcessor::*func)(String);
    };

    CalcKeyboard &keyboard;
    void cmdIsUp(String params);
    void cmdDiag(String params);
    void cmdKey(String params);
    void cmdSet(String params);
    void cmdGet(String params);
    void cmdRun(String params);
    void cmdCheck(String params);

    int commandCount;
    Command *commands;
};

#endif