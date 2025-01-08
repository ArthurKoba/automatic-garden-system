#ifndef AUTOMATIC_GARDEN_SYSTEM_UTILS_H
#define AUTOMATIC_GARDEN_SYSTEM_UTILS_H

#include "Arduino.h"
#include "RTClib.h"

void show_time(const DateTime& time_, const __FlashStringHelper *prefix = nullptr);

bool need_skip_task_iteration(
        uint32_t &last_iteration_time,
        uint32_t time_between_iterations_ms = 1000,
        bool forcibly = false
);

void show_bit_map(uint8_t flags);

bool is_valid_pin(int8_t pin, bool &error_flag, const __FlashStringHelper *pin_name = nullptr);

uint16_t get_average_analog_pin_value(uint8_t pin, uint8_t number_of_measurements = 5);

#endif //AUTOMATIC_GARDEN_SYSTEM_UTILS_H
