#ifndef AUTOMATIC_GARDEN_SYSTEM_APP_H
#define AUTOMATIC_GARDEN_SYSTEM_APP_H

#include "configs/configs.h"

#include "RTClib.h"
#include "GyverNTP.h"

#include "types.h"
#include "utils.h"

struct ErrorsAppInfo  {
    bool wifi_error_state : 1;
    bool wifi_not_connected_state : 1;
    bool ntp_error_state : 1;
    bool rtc_error_state : 1;
    bool rtc_bad_time_state : 1;
    bool pins_error_state: 1;
};

class AutomaticGarden {
    String _wifi_ssid;
    String _wifi_pass;
    wl_status_t _last_wifi_status = WL_NO_SHIELD;
    GardenPinsConfig _pin_cfg;
    ErrorsAppInfo _errors{};

    DateTime _disable_grow_lamp_time = DateTime(0, 0, 0, 23, 0, 0);
    DateTime _enable_grow_lamp_time = DateTime(0, 0, 0, 7, 0, 0);

    RTC_DS1307 _rtc;
    ESP8266WiFiClass _wifi;
    GyverNTP *_ntp = nullptr;

    void _on_connect_wifi();

    void _on_disconnect_wifi();

    void _check_wifi_task();

    void _check_rtc_task(bool forcibly = false);

    void _update_time_task(bool forcibly = false);

    void _update_lamps_states_task(bool forcibly = false);

    void _set_default_states_on_pins(bool show_errors = false);
public:
    AutomaticGarden();

    void setup(GardenPinsConfig pin_configs = {});

    void loop();
    ErrorsAppInfo get_errors_info();
};

#endif //AUTOMATIC_GARDEN_SYSTEM_APP_H
