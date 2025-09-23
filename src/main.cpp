#include <Arduino.h>

#include "app.h"

auto app = Application();

__attribute__((used)) void setup() {
    Serial.begin(SERIAL_SPEED);
    delay(100);
    Serial.println();
    app.setup();
}

__attribute__((used)) void loop() {
    app.loop();
}