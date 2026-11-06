#include "Led.h"

Led::Led(int pin, int id)
    : pin(pin), id(id)
{
}

void Led::begin()
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void Led::setState(bool state)
{
    digitalWrite(
        pin,
        state ? HIGH : LOW
    );
}

int Led::getId()
{
    return id;
}

int Led::getPin()
{
    return pin;
}