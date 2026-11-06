#pragma once

#include <Arduino.h>

class Led
{
public:

    explicit Led(int pin, int id);

    void begin();

    void setState(bool state);

    int getId();
    int getPin();

private:

    int pin;
    int id;
};  