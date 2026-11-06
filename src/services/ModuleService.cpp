#include "ModuleService.h"
#include "HardwareManager.h"
#include "generated/hardware.pb.h"
#include "config.h"

void ModuleService::handleMessage(ModuleMessage& message)
{
    switch (message.which_payload)
    {
        case ModuleMessage_set_led_tag:
            handleSetLed(message);
            break;

        case ModuleMessage_heartbeat_ack_tag:
            // Serial.println("Received heartbeat ack");
            break;

        case ModuleMessage_module_setup_tag:
            hardwareManager.setPins(message);
            break;

        default:
            Serial.println("Unknown message");
            break;
    }
}

void ModuleService::sendButtonPressed(
    int buttonId)
{
    ModuleMessage message =
        ModuleMessage_init_zero;

    // Identify which ESP32 sent the message
    strcpy(
        message.device_id,
        DEVICE_ID
    );

    // Tell protobuf which oneof field we're using
    message.which_payload = ModuleMessage_input_message_tag;

    // Set button information
    message.payload.input_message.inputId = buttonId;
    message.payload.input_message.which_input = InputMessage_button_pressed_tag;

    Serial.print("Sending ButtonPressed: ");
    Serial.println(buttonId);

    connection.sendMessage(message);
}

void ModuleService::sendButtonReleased(
    int buttonId)
{
    ModuleMessage message =
        ModuleMessage_init_zero;

    strcpy(
        message.device_id,
        DEVICE_ID
    );

    // Tell protobuf which oneof field we're using
    message.which_payload = ModuleMessage_input_message_tag;

    // Set button information
    message.payload.input_message.inputId = buttonId;
    message.payload.input_message.which_input = InputMessage_button_released_tag;

    Serial.print("Sending ButtonReleased: ");
    Serial.println(buttonId);

    connection.sendMessage(message);
}

void ModuleService::sendSetupRequest()
{
    ModuleMessage message =
        ModuleMessage_init_zero;

    strcpy(
        message.device_id,
        DEVICE_ID
    );

    message.which_payload = ModuleMessage_module_setup_request_tag;

    Serial.println("Sending SetupRequest");

    connection.sendMessage(message);
}

void ModuleService::handleSetLed(const ModuleMessage& message)
{
    const auto& led =
        message.payload.set_led;

    Serial.printf("Led state to be changed: %d, State: %d", led.led_id, led.state);
    
    digitalWrite(
        hardwareManager.getLedById(led.led_id).getPin(),
        led.state ? HIGH : LOW
    );
}