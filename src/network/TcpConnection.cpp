#include "TcpConnection.h"

#include <pb_encode.h>
#include <pb_decode.h>
#include "generated/hardware.pb.h"
#include <config.h>
#include <cstring>
#include <vector>

TcpConnection::TcpConnection(const char *host, uint16_t port)
    : host(host), port(port), lastHeartbeat(0)
{
}

bool TcpConnection::connect()
{
    if (client.connected())
    {
        return true;
    }

    Serial.print("Connecting to RoomEngine at ");
    Serial.print(host);
    Serial.print(":");
    Serial.println(port);

    if (!client.connect(host, port))
    {
        Serial.println("Failed to connect to RoomEngine!");

        return false;
    }

    Serial.println("Connected to RoomEngine!");

    return true;
}

void TcpConnection::disconnect()
{
    if (client.connected())
    {
        client.stop();

        Serial.println("Disconnected from RoomEngine");
    }
}

void TcpConnection::update()
{
    if (!client.connected())
    {
        return;
    }

    unsigned long now = millis();

    if (now - lastHeartbeat >= HEARTBEAT_INTERVAL)
    {
        sendHeartbeat();
        lastHeartbeat = now;
    }
}

void TcpConnection::sendHeartbeat()
{
    ModuleMessage message =
        ModuleMessage_init_zero;

    strncpy(
        message.device_id,
        DEVICE_ID,
        sizeof(message.device_id) - 1);

    message.which_payload = ModuleMessage_heartbeat_tag;

    if (!sendMessage(message))
    {
        Serial.println("Failed to send heartbeat");
    }
}

bool TcpConnection::isConnected()
{
    return client.connected();
}

bool TcpConnection::sendMessage(const ModuleMessage &message)
{
    if (!client.connected())
    {
        Serial.println("Cannot send message: not connected");

        return false;
    }

    // Buffer for encoded protobuf message
    uint8_t buffer[128];

    pb_ostream_t stream =
        pb_ostream_from_buffer(
            buffer,
            sizeof(buffer));

    // Encode protobuf
    if (!pb_encode(&stream, ModuleMessage_fields, &message))
    {
        Serial.print("Failed to encode protobuf: ");
        Serial.println(PB_GET_ERROR(&stream));

        return false;
    }

    uint32_t messageSize = static_cast<uint32_t>(stream.bytes_written);

    // Create 4-byte big-endian message size
    uint8_t header[4]{
        static_cast<uint8_t>((messageSize >> 24) & 0xFF),
        static_cast<uint8_t>((messageSize >> 16) & 0xFF),
        static_cast<uint8_t>((messageSize >> 8) & 0xFF),
        static_cast<uint8_t>(messageSize & 0xFF)};

    // Send message size
    if (!sendAll(header, sizeof(header)))
    {
        Serial.println("Failed to send message header");

        return false;
    }

    // Send protobuf data
    if (!sendAll(buffer, messageSize))
    {
        Serial.println("Failed to send protobuf message");

        return false;
    }

    Serial.print("Sent protobuf message (");
    Serial.print(messageSize);
    Serial.println(" bytes)");

    return true;
}

bool TcpConnection::receiveMessage(ModuleMessage &message)
{
    if (!client.connected())
    {
        return false;
    }

    // Wait until at least the 4-byte header is available
    if (client.available() < 4)
    {
        return false;
    }

    // Receive 4-byte message size
    uint8_t header[4];

    if (!receiveAll(header, sizeof(header)))
    {
        Serial.println("Failed to receive message header");

        return false;
    }

    // Convert big-endian bytes into uint32_t
    uint32_t messageSize =
        (static_cast<uint32_t>(header[0]) << 24) |
        (static_cast<uint32_t>(header[1]) << 16) |
        (static_cast<uint32_t>(header[2]) << 8) |
        static_cast<uint32_t>(header[3]);

    // Sanity check
    if (messageSize == 0 || messageSize > 1024)
    {
        Serial.print("Invalid message size: ");
        Serial.println(messageSize);

        disconnect();

        return false;
    }

    // Receive protobuf data
    // uint8_t buffer[1024];

    // if (!receiveAll(buffer, messageSize))
    // {
    //     Serial.println("Failed to receive protobuf message");

    //     return false;
    // }

    // // Create protobuf input stream
    // pb_istream_t stream =
    //     pb_istream_from_buffer(
    //         buffer,
    //         messageSize);

    std::vector<uint8_t> buffer(messageSize);

    if (!receiveAll(buffer.data(), messageSize))
    {
        Serial.println("Failed to receive protobuf message");
        return false;
    }

    pb_istream_t stream =
        pb_istream_from_buffer(buffer.data(), messageSize);

    // Decode protobuf
    if (!pb_decode(
            &stream,
            ModuleMessage_fields,
            &message))
    {
        Serial.print("Failed to decode protobuf: ");
        Serial.println(PB_GET_ERROR(&stream));

        return false;
    }

    // Serial.print("Received protobuf message (");
    // Serial.print(messageSize);
    // Serial.println(" bytes)");

    return true;
}

bool TcpConnection::sendAll(const uint8_t *buffer, size_t size)
{
    size_t totalSent = 0;

    while (totalSent < size)
    {
        int bytesSent = client.write(buffer + totalSent, size - totalSent);

        if (bytesSent <= 0)
        {
            return false;
        }

        totalSent += bytesSent;
    }

    return true;
}

bool TcpConnection::sendIdentification()
{
    ModuleMessage message =
        ModuleMessage_init_zero;

    // Retrieve the ESP32's actual MAC address.
    String macAddress = WiFi.macAddress();

    // Set the identification message.
    message.which_payload =
        ModuleMessage_setup_request_tag;

    strncpy(
        message.payload.setup_request.mac_address,
        macAddress.c_str(),
        sizeof(message.payload.setup_request.mac_address) - 1
    );

    Serial.print("Sending module identification: ");
    Serial.println(macAddress);

    if (!sendMessage(message))
    {
        Serial.println("Failed to send module identification");
        return false;
    }

    Serial.println("Module identification sent!");

    return true;
}

bool TcpConnection::receiveAll(uint8_t *buffer, size_t size)
{
    size_t totalReceived = 0;

    while (totalReceived < size)
    {
        if (!client.connected() && !client.available())
        {
            return false;
        }

        int bytesReceived = client.read(buffer + totalReceived, size - totalReceived);

        if (bytesReceived < 0)
        {
            return false;
        }

        if (bytesReceived == 0)
        {
            delay(1);
            continue;
        }

        totalReceived += bytesReceived;
    }

    return true;
}