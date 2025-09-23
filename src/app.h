#ifndef AUTOMATIC_GARDEN_SYSTEM_APP_H
#define AUTOMATIC_GARDEN_SYSTEM_APP_H

#include <GyverDBFile.h>
#include <LittleFS.h>
#include <SettingsESP.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFi.h>
#include <RTClib.h>
#include <GyverNTP.h>

#include "task.h"
#include "utils.h"
#include "configs.h"


DB_KEYS(
        kk,
        wifi_ssid,
        wifi_pass,
        apply_wifi_params,
        system_enabled,
        led_enabled,
        led_color,
        grow_lamp_status,
        grow_lamp_enabled,
        rtc_time,
        ntp_time,
        pins_error,
        rtc_state_error,
        rtc_bad_time_error,
        ntp_state_error,
        disable_grow_lamp_time,
        enable_grow_lamp_time
)

struct ErrorsAppInfo  {
    bool rtc_error_state : 1;
    bool rtc_bad_time_state : 1;
    bool pins_error_state: 1;
};

struct GardenPinsConfig {
    int8_t grow_lamp = GROW_LAMP_PIN;
    int8_t soil_moisture_analog = SOIL_MOISTURE_APIN;
    int8_t red_light_lamp = RED_LIGHT_PIN;
    int8_t green_light_lamp = GROW_LAMP_PIN;
    int8_t blue_light_lamp = BLUE_LIGHT_PIN;
};

class Application {
private:
    GardenPinsConfig _pin_cfg;
    GyverDBFile _db = GyverDBFile(&LittleFS, "/garden_system.db");
    SettingsESP _settings = SettingsESP(PROJECT_NAME, &_db);

    bool _system_enabled = false;
    bool _grow_lamp_enabled = false;
//    bool _led_enabled = false;
//    uint32_t _led_color = 0;

    DateTime _last_rtc_time;
    DateTime _last_ntp_time;
    DateTime _disable_grow_lamp_time = DateTime(0, 0, 0, 23, 0, 0);
    DateTime _enable_grow_lamp_time = DateTime(0, 0, 0, 7, 0, 0);
    RTC_DS1307 _rtc;
    GyverNTP *_ntp = nullptr;


    Task _wifi_connect_task = Task(1000);
    Task _update_grow_lamp_value_task = Task(1000);
//    Task _update_led_value_task = Task(100);
    Task _check_rtc_task = Task(1000);
    Task _check_ntp_task = Task(10000);
    Task _error_blink_task = Task(100);

    std::vector<Task> _tasks;
    bool _set_default_states_on_pins(bool show_errors = true) const;

    void _set_grow_lamp_value(bool value);
    void _set_led_value_and_color(bool value, uint32_t color);
    void _set_system_enables_value(bool value);
public:
    void setup();

    void loop();
};

#endif //AUTOMATIC_GARDEN_SYSTEM_APP_H
