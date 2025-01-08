#include "app.h"

void AutomaticGarden::_on_connect_wifi() {
    Serial.print(F("Connected to Wi-Fi. IP: "));
    Serial.println(_wifi.localIP().toString().c_str());
    _errors.wifi_error_state = false;
    _update_time_task(true);
}

void AutomaticGarden::_on_disconnect_wifi() {
    Serial.print(F("Disconnected from Wi-Fi."));
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

void AutomaticGarden::_check_rtc_task(bool forcibly) {
    static uint32_t last_exec = 0;
    static uint32_t delay_iter_ms = 1000;
    if (need_skip_task_iteration(last_exec, delay_iter_ms, forcibly)) return;

    if (not _rtc.isrunning() and not _rtc.begin()) {
        Serial.println(F("Failed running RTC"));
        _errors.rtc_error_state = true;
        return;
    } else if (not _rtc.now().isValid()) {
        Serial.println(F("Bad rtc time"));
        _errors.rtc_error_state = true;
        return;
    } else _errors.rtc_error_state = false;

    if (not _rtc.now().isValid() or _rtc.now().year() < 2020 or _rtc.now().year() > 2030) {
        _errors.rtc_bad_time_state = true;
        if (delay_iter_ms not_eq 1001) {
            Serial.println(F("Current year less 2020 or more 2030. Skip update lamps states task!"));
        }
        delay_iter_ms = 1001;
        return;
    } else _errors.rtc_bad_time_state = false;
    delay_iter_ms = 1000;
}

void AutomaticGarden::_update_time_task(bool forcibly) {
    static uint32_t last_exec = 0;
    static uint32_t delay_iter_ms = 1000;
    if (need_skip_task_iteration(last_exec, delay_iter_ms, forcibly)) return;

    if (_errors.wifi_not_connected_state) return;

    bool ntp_errors = false;
    if (not _ntp) {
        _ntp = &NTP;
        _ntp->setHost(NTP_SERVER_DOMAIN);

        if (not _ntp->begin()) {
            Serial.println(F("NTP not started"));
            _ntp = nullptr;
        }
    }
    if (not _ntp) {
        ntp_errors = true;
    } else if (not _ntp->updateNow()) {
        Serial.println(F("NTP failed get time!"));
        ntp_errors = true;
    } else if (_ntp->hasError()) {
        Serial.print(F("NTP error: "));
        Serial.println(_ntp->readError());
        if (_ntp->getError() not_eq GyverNTPClient::Error::Timeout) {
            delay_iter_ms = 1000 * 60 * 60;
            ntp_errors = true;
        }
    }

    if (ntp_errors) {
        _errors.ntp_error_state = true;
        return;
    } _errors.ntp_error_state = false;

    if (_errors.rtc_error_state) return;

    auto date_time_rtc = _rtc.now();
    auto date_time_ntp = DateTime(_ntp->getUnix() + 3600 * TIMEZONE_OFFSET);

    auto delta = float(date_time_ntp.unixtime()) - float(date_time_rtc.unixtime());

    if (delay_iter_ms == 1000) show_time(date_time_ntp, F("The first time received from NTP: "));

    delay_iter_ms = _errors.rtc_bad_time_state ? 1000 : 1000 * 60;

    if (abs(delta) < 2) {
        _errors.rtc_bad_time_state = false;
        return;
    };
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
    static uint32_t delay_iter_ms = 100;
    if (need_skip_task_iteration(last_exec, delay_iter_ms, forcibly)) return;

    if (_errors.rtc_error_state or _errors.rtc_bad_time_state) {
        return _set_default_states_on_pins(false);
    };

    auto current_time = DateTime(0, 0, 0,_rtc.now().hour(), _rtc.now().minute(), _rtc.now().second());

    if (current_time >= _enable_grow_lamp_time and current_time < _disable_grow_lamp_time) {
        digitalWrite(_pin_cfg.grow_lamp, true);
    } else digitalWrite(_pin_cfg.grow_lamp, false);

}

void AutomaticGarden::_check_soil_moisture_task(bool forcibly) {
    static uint32_t last_exec = 0;
    static uint32_t delay_iter_ms = 100;
    if (need_skip_task_iteration(last_exec, delay_iter_ms, forcibly)) return;

    if (_errors.pins_error_state) return;

    auto avg_value = get_average_analog_pin_value(_pin_cfg.soil_moisture_analog);

    if (avg_value < 10) {
        if (delay_iter_ms not_eq 1001)  Serial.println(F("Soil Moisture pin not connected!"));
        delay_iter_ms = 1001;
        return;
    }
    delay_iter_ms = 1000 * 60;

    Serial.printf("Soil Moisture pin value: %u\n", avg_value);
}

void AutomaticGarden::_set_default_states_on_pins(bool show_errors) {
    bool error_pins = false;

    if (is_valid_pin(_pin_cfg.grow_lamp, error_pins, show_errors ? F("grow lamp") : nullptr)) {
        pinMode(_pin_cfg.grow_lamp, OUTPUT);
        digitalWrite(_pin_cfg.grow_lamp, false);
    }

    if (is_valid_pin(_pin_cfg.soil_moisture_analog, error_pins, show_errors ? F("soil moisture") : nullptr)) {
        pinMode(_pin_cfg.soil_moisture_analog, INPUT);
    }

    if (is_valid_pin(_pin_cfg.red_light_lamp, error_pins, show_errors ? F("red light lamp") : nullptr)) {
        pinMode(_pin_cfg.red_light_lamp, OUTPUT);
        digitalWrite(_pin_cfg.red_light_lamp, false);
    }
    if (is_valid_pin(_pin_cfg.green_light_lamp, error_pins, show_errors ? F("green light lamp") : nullptr)) {
        pinMode(_pin_cfg.green_light_lamp, OUTPUT);
        digitalWrite(_pin_cfg.green_light_lamp, false);
    }

    if (is_valid_pin(_pin_cfg.blue_light_lamp, error_pins, show_errors ? F("blue light lamp") : nullptr)) {
        pinMode(_pin_cfg.blue_light_lamp, OUTPUT);
        digitalWrite(_pin_cfg.blue_light_lamp, false);
    }

    _errors.pins_error_state = error_pins;
}

AutomaticGarden::AutomaticGarden() {
    _wifi_ssid = String(F(WIFI_SSID));
    _wifi_pass = String(F(WIFI_PASS));

    _errors.wifi_not_connected_state = true;
    _errors.wifi_error_state = true;
    _errors.ntp_error_state = true;
    _errors.rtc_error_state = true;
    _errors.rtc_bad_time_state = true;
    _errors.pins_error_state = true;
}

void AutomaticGarden::setup(GardenPinsConfig pin_configs) {
    Serial.println(F("Run Automatic Garden System controller\n"));

    _pin_cfg = pin_configs;
    _set_default_states_on_pins(true);

    if (_wifi_ssid.length()) _last_wifi_status = WiFi.begin(_wifi_ssid, _wifi_pass);
    else Serial.println(F("ERROR: wifi ssid not set. Trying connect not possible!"));

    if (_rtc.begin()) {
        _check_rtc_task(true);
        _update_lamps_states_task(true);
    } else {
        Serial.println(F("Couldn't find RTC"));
    }
}

void AutomaticGarden::loop() {
    _check_rtc_task();
    _check_wifi_task();
    _update_time_task();
    _update_lamps_states_task();
    _check_soil_moisture_task();
}

ErrorsAppInfo AutomaticGarden::get_errors_info() {
    return _errors;
}
