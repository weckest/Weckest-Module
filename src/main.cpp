#include <Arduino.h>
#include <WiFi.h>

#include "config.h"
#include "network/TcpConnection.h"
#include "hardware/button.h"
#include "hardware/led.h"
#include "services/ModuleService.h"
#include "HardwareManager.h"

TcpConnection connection(ROOM_ENGINE_HOST, ROOM_ENGINE_PORT);
HardwareManager hardwareManager;
ModuleService moduleService(connection, hardwareManager);


void setup()
{

    Serial.begin(115200);

    Serial.println("Starting Module...");


    hardwareManager.setModuleService(moduleService);

    hardwareManager.begin();

    Serial.println("Connecting to Wi-Fi...");

    Serial.println(WIFI_SSID);
    Serial.println(WIFI_PASSWORD);

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi connected!");

    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.macAddress());

    connection.connect();
}

void loop()
{
    if (!connection.isConnected())
    {
        connection.connect();

        delay(1000);

        return;
    }

    connection.update();

    hardwareManager.update();

    ModuleMessage message =
        ModuleMessage_init_zero;

    if (connection.receiveMessage(message))
    {
        moduleService.handleMessage(message);
    }

    delay(10);
}