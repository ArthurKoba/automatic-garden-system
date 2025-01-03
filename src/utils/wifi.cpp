#include "configs/configs.h"
#include "wifi.h"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#error Unknown platform
#endif



void connect_to_wifi() {
    wl_status_t status = WiFi.begin(WIFI_SSID, WIFI_PASS);
    wl_status_t new_status;
    do {
        new_status = WiFi.status();
        if (status != new_status) {
            Serial.printf("New status wi-fi %u\n", status);
        }
        status = new_status;
        digitalWrite(LED_PIN, digitalRead(LED_PIN));
        delay(100);
    } while (status != WL_CONNECTED);
    Serial.printf("Wi-Fi ip: %s\n", WiFi.localIP().toString().c_str());
    digitalWrite(LED_PIN, false);
}


