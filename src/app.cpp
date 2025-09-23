#include "app.h"

void Application::setup() {
    Serial.print(F("Start project "));
    Serial.println(F(PROJECT_NAME));

#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif
    WiFi.mode(WIFI_AP_STA);
    _settings.begin();
    _settings.onBuild([this](sets::Builder &b) {
        {
            sets::Group g(b, "System statuses");
            b.LED(kk::system_enabled, "System enabled");
            b.LED(kk::grow_lamp_status, "Grow lamp status");
            b.Label(kk::rtc_time, "RTC time");
            b.Label(kk::ntp_time, "NTP time");

        }
        {
            sets::Group g(b, "System errors");
            b.Label(kk::pins_error, "Pins errors");
            b.Label(kk::rtc_state_error, "RTC error state");
            b.Label(kk::rtc_bad_time_error, "RTC bad time");
            b.Label(kk::ntp_state_error, "NTP error state");
        }
        {
            sets::Group g(b, "System");
            if (b.Switch("Enabled", &_system_enabled)) _set_system_enables_value(_system_enabled);
//            if (b.Switch("Led enabled", &_led_enabled)) {
//                _db[kk::led_enabled] = _led_enabled;
//            };
//            if (b.Color(kk::led_color, "Led Color", &_led_color)) {
//                printf("%u \n", _led_color);
//            }

            if (b.Switch("Grow lamp enabled", &_grow_lamp_enabled)) _set_grow_lamp_value(_grow_lamp_enabled);
            if (b.Time(kk::enable_grow_lamp_time, "Enable grow lamp time")) {
                _enable_grow_lamp_time = get_date_time_from_seconds(_db[kk::enable_grow_lamp_time]);
            };
            if (b.Time(kk::disable_grow_lamp_time, "Disable grow lamp time")) {
                _disable_grow_lamp_time = get_date_time_from_seconds(_db[kk::disable_grow_lamp_time]);
            };
        }

        {
            sets::Group g(b, "WiFi");
            b.Input(kk::wifi_ssid, "SSID");
            b.Pass(kk::wifi_pass, "Password");
            if (b.Button(kk::apply_wifi_params, "Save & Restart")) {
                _db.update();  // сохраняем БД не дожидаясь таймаута
                EspClass::restart();
            }
        }
    });
    _settings.onUpdate([this](sets::Updater &u) {
        _db[kk::rtc_time] = get_time(_last_rtc_time);
        _db[kk::ntp_time] = get_time(_last_ntp_time);
    });

    _db.begin();
    _db.init(kk::wifi_ssid, "");
    _db.init(kk::wifi_pass, "");
    _db.init(kk::system_enabled, _system_enabled);
    _set_system_enables_value(_db[kk::system_enabled]);

    _db.init(kk::grow_lamp_enabled, true);
    _db.init(kk::grow_lamp_status, false);
    _db[kk::grow_lamp_status] = false;
    _set_grow_lamp_value(_db[kk::grow_lamp_enabled]);

    _db.init(kk::led_enabled, false);
    _db.init(kk::led_color, uint32_t(255 << 16) + 255);
    _set_led_value_and_color(_db[kk::led_enabled], _db[kk::led_color]);

    _db.init(kk::disable_grow_lamp_time, 23 * 60 * 60);
    _db.init(kk::enable_grow_lamp_time, 7 * 60 * 60);

    _disable_grow_lamp_time = get_date_time_from_seconds(_db[kk::disable_grow_lamp_time]);
    _enable_grow_lamp_time = get_date_time_from_seconds(_db[kk::enable_grow_lamp_time]);

    _db[kk::rtc_state_error] = not _rtc.begin();
    _db[kk::pins_error] = _set_default_states_on_pins();

    if (not _db[kk::rtc_state_error] and is_valid_time(_rtc.now())) {
        _last_rtc_time = _rtc.now();
    } else {
        _db[kk::rtc_bad_time_error] = true;
    }

    _set_system_enables_value(_db[kk::system_enabled]);

    _wifi_connect_task.set_task_function([this](Task &current_task) {
        static size_t tries = _db[kk::wifi_ssid].length() ? 20 : 0;
        if (tries == 20) {
            printf("Try connect to WiFi... SSID: %s\n", _db[kk::wifi_ssid].toString().c_str());
            WiFi.begin(_db[kk::wifi_ssid], _db[kk::wifi_pass]);
        } else if (tries == 1) {
            printf("Failed connect to WiFI...\n");
        } else if (not tries) {
            printf("Enable STA: %s\n", PROJECT_NAME);
            WiFi.softAP(PROJECT_NAME);
            return current_task.disable();
        } else if (WiFi.status() == WL_CONNECTED) {
            printf("Connected to WiFI. IP: %s\n", WiFi.localIP().toString().c_str());
            return current_task.disable();
        } else {
            printf("Try connect to wifi: %d\n", tries);
        }
        tries--;
    });

    _update_grow_lamp_value_task.set_task_function([this](Task &current_task) {
        if (not _system_enabled or not _grow_lamp_enabled or not is_valid_time(_last_rtc_time)) {
            _db[kk::grow_lamp_status] = false;
            digitalWrite(_pin_cfg.grow_lamp, false);
            return;
        }
        auto current_time = DateTime(0, 0, 0, _last_rtc_time.hour(), _last_rtc_time.minute(), _last_rtc_time.second());
        bool status = current_time >= _enable_grow_lamp_time and current_time < _disable_grow_lamp_time;
        _db[kk::grow_lamp_status] = status;
        digitalWrite(_pin_cfg.grow_lamp, status);
    });

    _check_rtc_task.set_task_function([this](Task &current_task) {
        if (not _rtc.isrunning() and not _rtc.begin()) {
            Serial.println(F("Failed running RTC"));
            _db[kk::rtc_state_error] = true;
            return;
        } else if (not is_valid_time(_rtc.now())) {
            _db[kk::rtc_bad_time_error] = true;
        } else {
            _last_rtc_time = _rtc.now();
            _db[kk::rtc_bad_time_error] = false;
        }
        if (_db[kk::rtc_bad_time_error] and is_valid_time(_last_ntp_time)) {
            _rtc.adjust(_last_ntp_time);
        }
        _db[kk::rtc_state_error] = false;
    });

    _check_ntp_task.set_task_function([this](Task &current_task) {
        if (WiFi.status() not_eq WL_CONNECTED) return;


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
                ntp_errors = true;
            }
        }
        _db[kk::ntp_state_error] = ntp_errors;
        if (ntp_errors) return;
        if (not _rtc.isrunning()) return;

        auto date_time_rtc = _rtc.now();
        _last_ntp_time = DateTime(_ntp->getUnix() + 3600 * TIMEZONE_OFFSET);

        auto delta = float(_last_ntp_time.unixtime()) - float(date_time_rtc.unixtime());

        if (abs(delta) < 2) {
            _db[kk::rtc_bad_time_error] = true;
            return;
        };
        show_time(_last_ntp_time, F("Got time from NTP: "));
        show_time(date_time_rtc, F("Time from RTC: "));

        Serial.print(F("Delta time seconds: "));
        Serial.println(delta);
        _rtc.adjust(_last_ntp_time);
        Serial.println(F("Correction RTC time from NTP."));
    });

//    _update_led_value_task.set_task_function([this] (Task &current_task) {
//        if (not _system_enabled or not _led_enabled) {
//            digitalWrite(_pin_cfg.red_light_lamp, false);
//            digitalWrite(_pin_cfg.green_light_lamp, false);
//            digitalWrite(_pin_cfg.blue_light_lamp, false);
//            return;
//        }
//        uint8_t red = (_led_color >> 16) & 0x000000FF;
//        uint8_t green = (_led_color >> 8) & 0x000000FF;
//        uint8_t blue = _led_color & 0x000000FF;
//
//        analogWrite(_pin_cfg.red_light_lamp, red);
//        analogWrite(_pin_cfg.green_light_lamp, green);
//        analogWrite(_pin_cfg.blue_light_lamp, blue);
//    });

    _error_blink_task.set_task_function([this](Task &current_task) {
        bool have_errors = _db[kk::pins_error] or _db[kk::rtc_state_error] or _db[kk::rtc_bad_time_error] or _db[kk::ntp_state_error];
        current_task.set_delta_exec_ms(not have_errors ? 1000 : 100);
        digitalWrite(_pin_cfg.system, !digitalRead(_pin_cfg.system));
    });

    _tasks.push_back(_wifi_connect_task);
    _tasks.push_back(_update_grow_lamp_value_task);
//    _tasks.push_back(_update_led_value_task);
    _tasks.push_back(_check_rtc_task);
    _tasks.push_back(_error_blink_task);
}

void Application::loop() {
    _settings.tick();
    _db.tick();
    for (Task &task: _tasks) task.tick();
}

void Application::_set_grow_lamp_value(bool value) {
    _grow_lamp_enabled = value;
    _db[kk::grow_lamp_enabled] = _grow_lamp_enabled;
}


bool Application::_set_default_states_on_pins(bool show_errors) const {
    bool error_pins = false;

    if (is_valid_pin(_pin_cfg.grow_lamp, error_pins, show_errors ? F("grow lamp") : nullptr)) {
        pinMode(_pin_cfg.grow_lamp, OUTPUT);
        digitalWrite(_pin_cfg.grow_lamp, false);
    }

    if (is_valid_pin(_pin_cfg.system, error_pins, show_errors ? F("system") : nullptr)) {
        pinMode(_pin_cfg.system, OUTPUT);
        digitalWrite(_pin_cfg.system, false);
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
    return error_pins;
}


void Application::_set_system_enables_value(bool value) {
    _system_enabled = value;
    _db[kk::system_enabled] = _system_enabled;
}

void Application::_set_led_value_and_color(bool value, uint32_t color) {
//    _led_enabled = value;
//    _db[kk::led_enabled] = _led_enabled;
//    _led_color = color;
}
