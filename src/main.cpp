#include <Arduino.h>

#include "app.h"

auto app = AutomaticGarden();

__attribute__((used)) void setup() {
    pinMode(LED_PIN, OUTPUT);

    Serial.begin(SERIAL_BAUDRATE);
    delay(100);
    Serial.println();

    app.setup();
}

__attribute__((used)) void loop() {
    app.loop();

    static uint32_t last_check_app_state_time_ms = 0;
    static uint32_t delay_between_iterations = 100;
    if (need_skip_task_iteration(last_check_app_state_time_ms, delay_between_iterations)) return;
    auto errors = app.get_errors_info();
    bool have_errors = *reinterpret_cast<uint8_t *>(&errors) not_eq 0;

    static ErrorsAppInfo last_errors;
    if (last_check_app_state_time_ms > 5000 and
        *reinterpret_cast<uint8_t *>(&last_errors) not_eq *reinterpret_cast<uint8_t *>(&errors)) {
        last_errors = errors;
        Serial.print(F("Update app errors. Bitmap: "));
        show_bit_map(*reinterpret_cast<uint8_t *>(&last_errors));
    }


    delay_between_iterations = not have_errors ? 1000 : 100;
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}