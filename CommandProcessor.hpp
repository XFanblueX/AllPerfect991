#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H 

#include "CalcKeyboard.hpp"
#include <Arduino.h>
#include <SD.h>

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
    File file;
    void cmdIsUp(String params);
    void cmdDiag(String params);
    void cmdKey(String params);
    void cmdSet(String params);
    void cmdGet(String params);
    void cmdOpen(String params);
    void cmdSize(String params);
    void cmdCheck(String params);
    void cmdRun(String params);
    void cmdClose(String params);

    int commandCount;
    Command *commands;
};

#endif