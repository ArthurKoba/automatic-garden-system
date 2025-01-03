#include "configs/configs.h"

#include <Arduino.h>
#include "utils/wifi.h"


__attribute__((used)) void setup() {
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    connect_to_wifi();
}


__attribute__((used)) void loop() {
    Serial.println(123);
    delay(1000);
}