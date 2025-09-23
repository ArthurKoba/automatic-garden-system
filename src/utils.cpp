#include "utils.h"

void show_time(const DateTime &time_, const __FlashStringHelper *prefix) {
    if (prefix) Serial.print(prefix);
    static auto format = F("%.2i.%.2i.%.4lu %.2i:%.2i:%.2i (%lu)\n");
    Serial.printf(reinterpret_cast<const char *>(format),
                  time_.day(), time_.month(), time_.year(), time_.hour(), time_.minute(), time_.second(),
                  time_.unixtime()
    );
}

bool is_valid_time(const DateTime &time_) {
    return time_.isValid() and time_.year() > 2020 and time_.year() < 2030;
}

DateTime get_date_time_from_seconds(uint32_t seconds) {
    uint8_t hours = seconds / 3600;
    seconds = seconds % 3600;
    uint8_t minutes = seconds / 60;
    seconds = seconds % 60;
    return {0, 0, 0, hours, minutes, uint8_t(seconds)};
}

String get_time(const DateTime &time_) {
    if (not is_valid_time(time_)) return "NaN";
    String time;
    if (time_.hour() < 10) time += '0';
    time += time_.hour();
    time += ':';
    if (time_.minute() < 10) time += '0';
    time += time_.minute();
    time += ':';
    if (time_.second() < 10) time += '0';
    time += time_.second();
    return time;
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
    if (pin not_eq -1) return true;
    if (pin_name) {
        static auto format = F("ERROR: %s pin not installed!");
        Serial.printf(reinterpret_cast<const char *>(format), pin_name);
    }
    error_flag = true;
    return false;
}

uint16_t get_average_analog_pin_value(uint8_t pin, uint8_t number_of_measurements) {
    uint32_t sum = 0;
    for (int i = 0; i < number_of_measurements; ++i) sum += analogRead(pin);
    return sum /= number_of_measurements;
}



