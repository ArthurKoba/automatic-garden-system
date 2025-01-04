#include "app.h"

void AutomaticGarden::_on_connect_wifi() {
    Serial.print(F("Connected to Wi-Fi. IP: "));
    Serial.println(_wifi.localIP().toString().c_str());
    if (_ntp) return;
    _ntp = &NTP;
    _ntp->setHost(NTP_SERVER_DOMAIN);
    if (not _ntp->begin()) {
        Serial.print(F("Failed start NTP."));
        _ntp = nullptr;
        return;
    }
    _update_time_task(true);
}

void AutomaticGarden::_on_disconnect_wifi() {
    Serial.print(F("Disconnected from Wi-Fi."));
    if (not _ntp) return;
    _ntp = nullptr;
}

void AutomaticGarden::_wifi_task(bool forcibly) {
    static uint32_t last_exec = 0;
    if (need_skip_task_iteration(last_exec, 100, forcibly)) return;

    wl_status_t status = _wifi.status();

    if (status == _last_wifi_status) return;
    _last_wifi_status = status;

    switch (status) {
        case WL_CONNECTED:
            _on_connect_wifi();
            break;
        case WL_WRONG_PASSWORD:
            Serial.println(F("Wrong Wi-Fi password."));
            break;
        case WL_CONNECTION_LOST:
        case WL_DISCONNECTED:
            _on_disconnect_wifi();
            break;
        case WL_IDLE_STATUS:
        case WL_NO_SSID_AVAIL:
        case WL_SCAN_COMPLETED:
        case WL_NO_SHIELD:
        case WL_CONNECT_FAILED:
            break;
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
}

bool AutomaticGarden::need_skip_task_iteration(
        uint32_t &last_iteration_time,
        uint32_t time_between_iterations_ms,
        bool forcibly
) {
    auto current_time = millis();
    if (current_time - last_iteration_time < time_between_iterations_ms and not forcibly) return true;
    last_iteration_time = current_time;
    return false;
}

void AutomaticGarden::show_time(const DateTime &time_, const __FlashStringHelper *prefix) {
    if (prefix) Serial.print(prefix);
    static auto format = F("%.2i.%.2i.%.4lu %.2i:%.2i:%.2i (%lu)\n");
    Serial.printf(reinterpret_cast<const char *>(format),
                  time_.day(), time_.month(), time_.year(), time_.hour(), time_.minute(), time_.second(),
                  time_.unixtime()
    );
}

void AutomaticGarden::setup() {
    Serial.println(F("Run Automatic Garden System Controller"));
    _last_wifi_status = WiFi.begin(WIFI_SSID, WIFI_PASS);

    if (_rtc.begin()) {
        _current_time = _rtc.now();
        show_time(_current_time, F("Currently received from RTC unit: "));
    } else {
        Serial.println(F("Couldn't find RTC"));
    }
//    digitalWrite(GROW_LAMP_PIN, false);
}

void AutomaticGarden::loop() {
    _wifi_task();
    _update_time_task();
//    delay(100);
//    dt = rtc.now();

//    if (NTP.tick()) {
//        dt = DateTime(NTP.getUnix() + 3600 * TIMEZONE_OFFSET);
//        rtc.adjust(dt);
//        Serial.println("Update Datetime");
//    }
//    Serial.printf("%i:%i %i:%i:%i\n", dt.day(), dt.month(), dt.hour(), dt.minute(), dt.second());
//    auto value = analogRead(SOIL_MOISTURE_ANALOG_PIN);
//    Serial.println(value);
}
