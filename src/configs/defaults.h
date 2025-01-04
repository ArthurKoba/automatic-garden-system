#ifndef AUTOMATIC_GARDEN_SYSTEM_CONFIGS_DEFAULTS_H
#define AUTOMATIC_GARDEN_SYSTEM_CONFIGS_DEFAULTS_H

#if not defined(SERIAL_BAUDRATE)
#define SERIAL_BAUDRATE 1000000
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

#endif //AUTOMATIC_GARDEN_SYSTEM_CONFIGS_DEFAULTS_H
