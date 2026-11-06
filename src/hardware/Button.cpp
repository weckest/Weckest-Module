#include "Button.h"

Button::Button(int pin, int id) 
    : pin(pin),
    id(id),
    currentState(HIGH),
    previousState(HIGH)
{
}

void Button::begin()
{
    pinMode(pin, INPUT_PULLUP);
    
    currentState = digitalRead(pin);
    previousState = currentState;
}

void Button::start() {
    currentState = digitalRead(pin);
}

void Button::end() {
    previousState = currentState;
}

bool Button::wasPressed()
{
    bool pressed = currentState == LOW && previousState == HIGH;

    return pressed;
}

bool Button::wasReleased()
{
    bool released = currentState == HIGH && previousState == LOW;
    if(released) {
        Serial.print("Button released: ");
        Serial.println(id);
    }

    return released;
}

int Button::getId()
{
    return id;
}