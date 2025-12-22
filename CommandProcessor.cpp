#include "CommandProcessor.hpp"
#include "CalcKeyboard.hpp"
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

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

CommandProcessor::CommandProcessor(CalcKeyboard &kb) : keyboard(kb)
{

    commandCount = 10;
    commands = new Command[commandCount]{
        {"ISUP", &CommandProcessor::cmdIsUp},
        {"DIAG", &CommandProcessor::cmdDiag},
        {"KEY", &CommandProcessor::cmdKey},
        {"SET", &CommandProcessor::cmdSet},
        {"GET", &CommandProcessor::cmdGet},
        {"OPEN", &CommandProcessor::cmdOpen},
        {"SIZE", &CommandProcessor::cmdSize},
        {"CHECK", &CommandProcessor::cmdCheck},
        {"RUN", &CommandProcessor::cmdRun},
        {"CLOSE", &CommandProcessor::cmdClose}};
    Serial.begin(9600);
}

CommandProcessor::~CommandProcessor()
{
    file.close();
    delete[] commands;
}

void CommandProcessor::processCommand(String fullCommand)
{
    fullCommand.trim();
    DEBUG_PRINTLN("Starting command processor...");
    Serial.print("> ");
    Serial.println(fullCommand);

    int spaceIndex = fullCommand.indexOf(' ');
    String cmdName = fullCommand;
    String params = "";

    if (spaceIndex > 0)
    {
        cmdName = fullCommand.substring(0, spaceIndex);
        params = fullCommand.substring(spaceIndex + 1);
        params.trim();
    }

    cmdName.toUpperCase();

    DEBUG_PRINTLN(cmdName);

    // 查找并执行命令
    bool found = false;

    for (int i = 0; i < commandCount; i++)
    {
        DEBUG_PRINT(i);
        DEBUG_PRINTLN(commands[i].name);
        if (cmdName.equals(commands[i].name))
        {
            (this->*commands[i].func)(params);
            found = true;
            break;
        }
    }

    if (!found)
    {
        Serial.println("Invalid command");
    }
}

void CommandProcessor::cmdIsUp(String params)
{
    Serial.println("OK");
}

void CommandProcessor::cmdDiag(String params)
{
    Serial.print("Running diagnostic...");
    keyboard.runDiag();
    Serial.println("OK");
}

void CommandProcessor::cmdKey(String params)
{
    unsigned int keyValue;
    int count = sscanf(params.c_str(), "%x", &keyValue);

    bool valid = false;
    if (count == 1)
    {
        for (int i = 0; i < keyListSize; i++)
        {
            if (keyValue == keyList[i])
                valid = true;
        }
    }

    if (valid)
    {
        keyboard.press(keyValue);
        Serial.println("OK");
    }
    else
    {
        Serial.println("Invalid key value");
        return;
    }
}

void CommandProcessor::cmdSet(String params)
{
    int spaceIndex = params.indexOf(' ');

    if (spaceIndex <= 0)
    {
        Serial.println("No enough arguments");
        return;
    }

    String opt = params.substring(0, spaceIndex);
    String value = params.substring(spaceIndex + 1);

    int valueSet = value.toInt();

    if (keyboard.setOption(opt, valueSet))
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("Invalid option");
    }
}

void CommandProcessor::cmdGet(String params)
{
    Serial.println(keyboard.getOption(params));
}

void CommandProcessor::cmdOpen(String params)
{
    params.trim();

    if (!SD.begin())
    {
        Serial.println("Failed to open SD card");
        return;
    }
    DEBUG_PRINTLN("Before opening file");

    if (file)
    {
        file.close();
    }

    file = SD.open(params, FILE_READ);
    DEBUG_PRINTLN("File opened");
    DEBUG_PRINTLN(file);
    if (file)
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("Failed to open file");
    }
}

void CommandProcessor::cmdSize(String params)
{
    if (file)
    {
        Serial.println(file.size());
    }
    else
    {
        Serial.println("No file opened");
    }
}

void CommandProcessor::cmdCheck(String params)
{
    if (file)
    {
        file = SD.open(file.name(), FILE_READ);
        bool allValid = true;
        digitalWrite(LED_BUILTIN, HIGH);

        while (file.available())
        {
            byte value = file.read();

            if (!keyboard.checkKeyValue(value))
            {
                Serial.print("Invalid key value found at ");
                Serial.println(file.position(), HEX);
                Serial.print("Value: ");
                Serial.println(value, HEX);
                allValid = false;
                break;
            }
        }
        if (allValid)
        {
            Serial.println("OK");
        }
        digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
        Serial.println("No file opened");
    }
}

void CommandProcessor::cmdRun(String params)
{
    if (file)
    {
        file = SD.open(file.name(), FILE_READ);
        DEBUG_PRINTLN("File valid");
        while (file.available())
        {
            keyboard.press(file.read());
        }
        Serial.println("OK");
    }
    else
    {
        Serial.println("No file opened");
    }
}

void CommandProcessor::cmdClose(String params)
{
    if (file)
    {
        file.close();
        Serial.println("OK");
    }
    else
    {
        Serial.println("No file opened");
    }
}