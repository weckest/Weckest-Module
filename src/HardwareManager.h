#pragma once

#include <vector>
#include "hardware/Button.h"
#include "hardware/Led.h"
#include "generated/hardware.pb.h"

class ModuleService;

class HardwareManager {
public:
    explicit HardwareManager() {};

    void begin();

    void update();

    void addButton(int pin, int id);

    void addLed(int pin, int id);

    void setPins(ModuleMessage& message);

    Led& getLedById(int id) {
        for (Led& led : leds) {
            if (led.getId() == id) {
                return led;
            }
        }

        throw std::runtime_error("LED with the specified ID not found");
    }

    void setModuleService(ModuleService& moduleService) {
        this->service = &moduleService;
    }

private:

    void updateButtons();

    ModuleService* service;

    std::vector<Button> buttons;
    std::vector<Led> leds;

    bool setup = false;
};