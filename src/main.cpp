#include "configs/configs.h"

#include <Arduino.h>


__attribute__((used)) void setup() {
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
}


__attribute__((used)) void loop() {
    Serial.println(123);
    delay(1000);
}