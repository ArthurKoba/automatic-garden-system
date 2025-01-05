#ifndef AUTOMATIC_GARDEN_SYSTEM_APP_H
#define AUTOMATIC_GARDEN_SYSTEM_APP_H

#include "configs/configs.h"

#include "RTClib.h"
#include "GyverNTP.h"

class AutomaticGarden {
    RTC_DS1307 _rtc;
    ESP8266WiFiClass _wifi;
    GyverNTP *_ntp = nullptr;

    String _wifi_ssid;
    String _wifi_pass;
    DateTime _current_time;
    wl_status_t _last_wifi_status = WL_NO_SHIELD;


    void _on_connect_wifi();
    void _on_disconnect_wifi();
    void _wifi_task(bool forcibly = false);
    void _update_time_task(bool forcibly = false);

public:
    AutomaticGarden();
    void setup();
    void loop();
};

#endif //AUTOMATIC_GARDEN_SYSTEM_APP_H
