#pragma once

#include <Arduino.h>
#include <WiFi.h>

#include "generated/hardware.pb.h" 

class TcpConnection
{
public:
    TcpConnection(const char *host, uint16_t port);

    bool connect();

    void disconnect();

    bool isConnected();

    bool sendMessage(const ModuleMessage &message);

    bool receiveMessage(ModuleMessage &message);

    void update();

    void sendHeartbeat();

private:
    
    bool receiveAll(uint8_t *buffer, size_t size);

    bool sendAll(const uint8_t *buffer, size_t size);

    unsigned long lastHeartbeat;

    WiFiClient client;

    const char *host;
    uint16_t port;
};