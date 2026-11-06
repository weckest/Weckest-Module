#include "HardwareManager.h"
#include "services/ModuleService.h"
#include "generated/hardware.pb.h"
#include <pb_decode.h>

void HardwareManager::begin()
{
    for (Button &button : buttons)
    {
        button.begin();
    }

    for (Led &led : leds)
    {
        led.begin();
    }
}

void HardwareManager::update()
{
    if(buttons.empty() && setup == false)
    {
        if(service) {
            service->sendSetupRequest();
            setup = true;
            return;
        }
    }
    updateButtons();
}

void HardwareManager::updateButtons()
{
    for (Button &button : buttons)
    {
        button.start();
        if (button.wasPressed())
        {
            service->sendButtonPressed(button.getId());
        }

        if (button.wasReleased())
        {
            service->sendButtonReleased(button.getId());
        }
        button.end();
    }
}

void HardwareManager::addButton(int pin, int id)
{
    Serial.printf("Adding Button: pin=%d, id=%d\n", pin, id);
    buttons.emplace_back(Button{pin, id});
    buttons.back().begin();
}

void HardwareManager::addLed(int pin, int id)
{
    Serial.printf("Adding LED: pin=%d, id=%d\n", pin, id);
    leds.emplace_back(Led{pin, id});
    leds.back().begin();
}

void HardwareManager::setPins(ModuleMessage &message)
{
    if (message.which_payload != ModuleMessage_module_setup_tag)
    {
        return;
    }

    Serial.printf("Setting pins from ModuleSetup message\n");

    ModuleSetup& setup = message.payload.module_setup;

    for(pb_size_t i = 0; i < setup.input_map_count; ++i) {
        Serial.printf("Input[%u]: pin=%d, id=%d\n", i,
            setup.input_map[i].pin,
            setup.input_map[i].id
        );
        addButton(setup.input_map[i].pin, setup.input_map[i].id);
    }

    for(pb_size_t i = 0; i < setup.output_map_count; ++i) {
        Serial.printf("Output[%u]: pin=%d, id=%d\n", i,
            setup.output_map[i].pin,
            setup.output_map[i].id
        );
        addLed(setup.output_map[i].pin, setup.output_map[i].id);
    }
}
