#include <Arduino.h>

#include "app.h"

auto app = AutomaticGarden();

__attribute__((used)) void setup() {
    pinMode(LED_PIN, OUTPUT);

    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();

    app.setup();
}

__attribute__((used)) void loop() {
    app.loop();

    static uint32_t last_check_app_state_time_ms = 0;
    static uint32_t delay_between_iterations = 100;
    if (need_skip_task_iteration(last_check_app_state_time_ms, delay_between_iterations)) return;
    // todo check
    auto errors = app.get_errors_info();
    bool have_errors = *reinterpret_cast<uint32_t*>(&errors) not_eq 0;
    if (not have_errors) {
        delay_between_iterations = 1000;
        return digitalWrite(LED_PIN, true);
    }
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}