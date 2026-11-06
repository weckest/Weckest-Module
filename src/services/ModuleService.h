#pragma once

#include "generated/hardware.pb.h"
#include "network/TcpConnection.h"

class HardwareManager;

class ModuleService
{
public:

    ModuleService(TcpConnection& connection, HardwareManager& hardwareManager) 
    : connection(connection), hardwareManager(hardwareManager) {}

    void handleMessage(ModuleMessage& message);

    void sendButtonPressed(int buttonId);

    void sendButtonReleased(int buttonId);

    void sendSetupRequest();

    HardwareManager& hardwareManager;

private:

    void handleSetLed(const ModuleMessage& message);

    TcpConnection& connection;
    
};