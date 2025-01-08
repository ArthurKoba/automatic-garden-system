#include "utils.h"

void show_time(const DateTime& time_, const __FlashStringHelper *prefix) {
    if (prefix) Serial.print(prefix);
    static auto format = F("%.2i.%.2i.%.4lu %.2i:%.2i:%.2i (%lu)\n");
    Serial.printf(reinterpret_cast<const char *>(format),
          time_.day(), time_.month(), time_.year(), time_.hour(), time_.minute(), time_.second(),
          time_.unixtime()
    );
}

bool need_skip_task_iteration(uint32_t &last_iteration_time, uint32_t time_between_iterations_ms, bool forcibly) {
    auto current_time = millis();
    if (current_time - last_iteration_time < time_between_iterations_ms and not forcibly) return true;
    last_iteration_time = current_time;
    return false;
}

void show_bit_map(uint8_t flags) {
    for (int i = 0; i < 8; ++i) {
        Serial.print(bool((0x1 << i) & flags));
    }
    Serial.println();
}

bool is_valid_pin(int8_t pin, bool &error_flag, const __FlashStringHelper *pin_name) {
    if (pin not_eq -1) return false;
    if (pin_name) {
        static auto format = F("ERROR: %s pin not installed!");
        Serial.printf(reinterpret_cast<const char *>(format), pin_name);
    }
    return error_flag == true;
}
