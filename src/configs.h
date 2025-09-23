#ifndef AUTOMATIC_GARDEN_SYSTEM_CONFIGS_H
#define AUTOMATIC_GARDEN_SYSTEM_CONFIGS_H

#define PROJECT_NAME "Automatic Garden System"

#define SOIL_MOISTURE_APIN 17

#define GROW_LAMP_PIN 12

#define RED_LIGHT_PIN 14
#define GREEN_LIGHT_PIN 16
#define BLUE_LIGHT_PIN 13

#if not defined(SERIAL_SPEED)
#define SERIAL_SPEED 1000000
#endif

#if not defined(TIMEZONE_OFFSET)
#define TIMEZONE_OFFSET 0
#endif

#if not defined(NTP_SERVER_DOMAIN)
#define NTP_SERVER_DOMAIN "pool.ntp.org"
#endif

#if defined(ESP8266)
#define LED_PIN LED_BUILTIN
#else
#error Unknown platform
#endif

#if not defined(SOIL_MOISTURE_APIN)
#define SOIL_MOISTURE_APIN -1
#endif

#if not defined(GROW_LAMP_PIN)
#define GROW_LAMP_PIN -1
#endif

#if not defined(RED_LIGHT_PIN)
#define RED_LIGHT_PIN -1
#endif

#if not defined(GREEN_LIGHT_PIN)
#define GREEN_LIGHT_PIN -1
#endif

#if not defined(BLUE_LIGHT_PIN)
#define BLUE_LIGHT_PIN -1
#endif

#endif //AUTOMATIC_GARDEN_SYSTEM_CONFIGS_H
