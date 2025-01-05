#ifndef AUTOMATIC_GARDEN_SYSTEM_TYPES_H
#define AUTOMATIC_GARDEN_SYSTEM_TYPES_H

#include "configs/configs.h"

#include <Arduino.h>

struct GardenPinsConfig {
    int8_t grow_lamp = GROW_LAMP_PIN;
    int8_t soil_moisture_analog = SOIL_MOISTURE_APIN;
    int8_t red_light_lamp = RED_LIGHT_PIN;
    int8_t green_light_lamp = GROW_LAMP_PIN;
    int8_t blue_light_lamp = BLUE_LIGHT_PIN;
};

#endif //AUTOMATIC_GARDEN_SYSTEM_TYPES_H
