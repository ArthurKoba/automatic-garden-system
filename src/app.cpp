#include "app.h"

void AutomaticGarden::_on_connect_wifi() {
    Serial.print(F("Connected to Wi-Fi. IP: "));
    Serial.println(_wifi.localIP().toString().c_str());
    _errors.wifi_error_state = false;
    _update_time_task(true);
}

void AutomaticGarden::_on_disconnect_wifi() {
    Serial.print(F("Disconnected from Wi-Fi."));
    if (not _ntp) return;
    _ntp = nullptr;
}

void AutomaticGarden::_check_wifi_task() {
    static uint32_t last_exec = 0;
    if (need_skip_task_iteration(last_exec, 100)) return;

    wl_status_t status = _wifi.status();

    if (status == _last_wifi_status) return;
    _last_wifi_status = status;

    switch (status) {
        case WL_CONNECTED:
            _on_connect_wifi();
            _errors.wifi_not_connected_state = false;
            break;
        case WL_CONNECTION_LOST:
        case WL_DISCONNECTED:
            _on_disconnect_wifi();
            _errors.wifi_not_connected_state = true;
            break;
        case WL_IDLE_STATUS:
        case WL_SCAN_COMPLETED:
        case WL_WRONG_PASSWORD:
        case WL_NO_SSID_AVAIL:
        case WL_NO_SHIELD:
        case WL_CONNECT_FAILED:
            _errors.wifi_error_state = true;
            break;
    }
    if (status == WL_WRONG_PASSWORD) {
        Serial.println(F("Wrong Wi-Fi password."));
    }
}

void AutomaticGarden::_update_time_task(bool forcibly) {
    static uint32_t last_exec = 0;
    static uint32_t delay_iter_ms = 100;
    if (need_skip_task_iteration(last_exec, delay_iter_ms, forcibly)) return;

    if (not _rtc.isrunning() and not _rtc.begin()) {
        Serial.println(F("Failed running RTC"));
        delay_iter_ms = 100000;
        return;
    }

    if (_last_wifi_status not_eq WL_CONNECTED) return;

    if (not _ntp) {
        Serial.println(F("Failed running NTP Client"));
        delay_iter_ms = 1000 * 60 * 60;
        return;
    }

    if (_ntp->hasError()) {
        Serial.print("NTP Error: ");
        Serial.println(_ntp->readError());
        if (_ntp->getError() not_eq GyverNTPClient::Error::Timeout) delay_iter_ms = 1000 * 60 * 60;
    }

    if (not _ntp->updateNow()) return;

    auto date_time_ntp = DateTime(_ntp->getUnix() + 3600 * TIMEZONE_OFFSET);
    auto date_time_rtc = _rtc.now();

    auto delta = float(date_time_ntp.unixtime()) - float(date_time_rtc.unixtime());

    if (delay_iter_ms == 100) {
        show_time(date_time_ntp, F("The first time received from NTP: "));
    }

    delay_iter_ms = 1000 * 60;

    if (date_time_ntp.year() > 2000 and abs(delta) < 2) return;
    show_time(date_time_ntp, F("Got time from NTP: "));
    show_time(date_time_rtc, F("Time from RTC: "));

    Serial.print(F("Delta time seconds: "));
    Serial.println(delta);
    _rtc.adjust(date_time_ntp);
    Serial.println(F("Correction RTC time from NTP."));
    _update_lamps_states_task(true);
}

void AutomaticGarden::_update_lamps_states_task(bool forcibly) {
    static uint32_t last_exec = 0;
    static uint32_t delay_iter_ms = 1000;
    if (need_skip_task_iteration(last_exec, delay_iter_ms, forcibly)) return;

    if (not _rtc.isrunning() or _rtc.now().year() < 2000) {
        _errors.rtc_error_state = true;
        delay_iter_ms = 100000;
        if (delay_iter_ms not_eq 100000) {
            Serial.println(_rtc.isrunning() ?
                           F("Current year less 2000. Skip update lamps states task!") :
                           F("RTC unit not started. Skip update lamps states task!")
            );
        }
        return;
    };
    _errors.rtc_error_state = false;
    bool need_skip = true;

    if (_pin_cfg.grow_lamp not_eq -1) {
        need_skip = false;
        auto current_time = DateTime(0, 0, 0,
                                     _rtc.now().hour(), _rtc.now().minute(), _rtc.now().second()
        );

        if (current_time >= _enable_grow_lamp_time and current_time < _disable_grow_lamp_time) {
            digitalWrite(_pin_cfg.grow_lamp, true);
        } else digitalWrite(_pin_cfg.grow_lamp, false);
    }

    delay_iter_ms = need_skip ? 10000 : 100;
}

AutomaticGarden::AutomaticGarden() {
    _wifi_ssid = String(F(WIFI_SSID));
    _wifi_pass = String(F(WIFI_PASS));
}

void AutomaticGarden::setup(GardenPinsConfig pin_configs) {
    Serial.println(F("Run Automatic Garden System controller\n"));

    _pin_cfg = pin_configs;

    if (_pin_cfg.grow_lamp not_eq -1) pinMode(_pin_cfg.grow_lamp, OUTPUT);
    else Serial.println(F("ERROR: grow lamp pin not installed!"));

    if (_pin_cfg.soil_moisture_analog not_eq -1) pinMode(_pin_cfg.soil_moisture_analog, OUTPUT);
    else Serial.println(F("ERROR: soil moisture pin not installed!"));

    if (_pin_cfg.red_light_lamp not_eq -1) pinMode(_pin_cfg.red_light_lamp, OUTPUT);
    else Serial.println(F("ERROR: red light lamp pin not installed!"));

    if (_pin_cfg.green_light_lamp not_eq -1) pinMode(_pin_cfg.green_light_lamp, OUTPUT);
    else Serial.println(F("ERROR: green light lamp pin not installed!"));

    if (_pin_cfg.blue_light_lamp not_eq -1) pinMode(_pin_cfg.blue_light_lamp, OUTPUT);
    else Serial.println(F("ERROR: blue light lamp pin not installed!"));

    if (_wifi_ssid.length()) _last_wifi_status = WiFi.begin(_wifi_ssid, _wifi_pass);
    else Serial.println(F("ERROR: wifi ssid not set. Trying connect not possible!"));

    if (_rtc.begin()) {
        auto current_time = _rtc.now();
        show_time(current_time, F("Currently received from RTC unit: "));
        if (current_time.year() < 2000) Serial.println(F("Current year less 2000. Maybe RTC Unit battery low!"));
        else _update_lamps_states_task(true);
    } else {
        Serial.println(F("Couldn't find RTC"));
    }
}

void AutomaticGarden::loop() {
    _wifi_task();
    _check_wifi_task();
    _update_time_task();
    _update_lamps_states_task();
}

ErrorsAppInfo AutomaticGarden::get_errors_info() {
    return _errors;
}
