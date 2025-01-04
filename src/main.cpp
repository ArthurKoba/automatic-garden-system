
#include <Arduino.h>

#include "app.h"

auto app = AutomaticGarden();

__attribute__((used)) void setup() {
    pinMode(LED_PIN, OUTPUT);

    pinMode(GROW_LAMP_PIN, OUTPUT);

    pinMode(RED_LIGHT_PIN, OUTPUT);
    pinMode(GREEN_LIGHT_PIN, OUTPUT);
    pinMode(BLUE_LIGHT_PIN, OUTPUT);

    digitalWrite(RED_LIGHT_PIN, true);
    digitalWrite(GREEN_LIGHT_PIN, false);
    digitalWrite(BLUE_LIGHT_PIN, true);

    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();

    app.setup();
}

__attribute__((used)) void loop() {
    app.loop();
}