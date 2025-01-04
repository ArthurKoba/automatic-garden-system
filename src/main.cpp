#include "configs/configs.h"

#include <Arduino.h>
#include <RTClib.h>
#include "utils/wifi.h"
#include "utils/ntp.h"



GyverNTP ntp;
RTC_DS1307 rtc;
DateTime dt;


__attribute__((used)) void setup() {
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();

    connect_to_wifi();
    init_ntp(ntp);
    pinMode(LED_PIN, OUTPUT);
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
    }
//    digitalWrite(GROW_LAMP_PIN, false);
    //    update_rtc_time(&NTP, &rtc);
}




__attribute__((used)) void loop() {
    delay(100);
//    dt = rtc.now();

//    if (NTP.tick()) {
//        dt = DateTime(NTP.getUnix() + 3600 * TIMEZONE_OFFSET);
//        rtc.adjust(dt);
//        Serial.println("Update Datetime");
//    }
//    Serial.printf("%i:%i %i:%i:%i\n", dt.day(), dt.month(), dt.hour(), dt.minute(), dt.second());

}