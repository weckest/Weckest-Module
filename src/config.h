#pragma once

#define SIMULATED_ESP32 1

#define HEARTBEAT_INTERVAL 2000

#if SIMULATED_ESP32

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define ROOM_ENGINE_HOST "host.wokwi.internal"

#else

#define WIFI_SSID "Weckest-Room"
#define WIFI_PASSWORD "Weckest1!"
#define ROOM_ENGINE_HOST "10.42.0.1"

#endif


#define ROOM_ENGINE_PORT 5001

#define DEVICE_ID "24:0A:C4:00:01:10"