#pragma once

#include <Arduino.h>

class Button
{
public:

    explicit Button(int pin, int id);

    void begin();

    void start();

    void end();

    bool wasPressed();

    bool wasReleased();

    int getId();

private:

    int pin;
    int id;

    int currentState;
    int previousState;
};