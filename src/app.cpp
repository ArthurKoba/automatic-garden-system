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

void AutomaticGarden::_wifi_task() {
    static uint32_t last_exec = 0;
    if (need_skip_task_iteration(last_exec, 100)) return;

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

AutomaticGarden::AutomaticGarden() {
    _wifi_ssid = String(F(WIFI_SSID));
    _wifi_pass = String(F(WIFI_PASS));
}

void AutomaticGarden::setup() {
    Serial.println(F("Run Automatic Garden System controller\n"));

    if (_wifi_ssid.length()) _last_wifi_status = WiFi.begin(_wifi_ssid, _wifi_pass);
    else Serial.println(F("ERROR: wifi ssid not set. Trying connect not possible!"));

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
