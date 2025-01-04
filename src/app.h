#ifndef AUTOMATIC_GARDEN_SYSTEM_APP_H
#define AUTOMATIC_GARDEN_SYSTEM_APP_H

#include "configs/configs.h"

#include "RTClib.h"
#include "GyverNTP.h"

class AutomaticGarden {
    RTC_DS1307 _rtc;
    ESP8266WiFiClass _wifi;
    GyverNTP *_ntp = nullptr;

    DateTime _current_time;
    wl_status_t _last_wifi_status;

    void _on_connect_wifi();
    void _on_disconnect_wifi();
    void _wifi_task(bool forcibly = false);
    void _update_time_task(bool forcibly = false);

    static bool need_skip_task_iteration(
            uint32_t &last_iteration_time,
            uint32_t time_between_iterations_ms = 1000,
            bool forcibly = false
    );
public:
    void setup();
    void loop();
};

#endif //AUTOMATIC_GARDEN_SYSTEM_APP_H
