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


    pinMode(LED_PIN, OUTPUT);
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
    }
    pinMode(GROW_LAMP_PIN, OUTPUT);
    pinMode(RED_LIGHT_PIN, OUTPUT);
    pinMode(GREEN_LIGHT_PIN, OUTPUT);
    pinMode(BLUE_LIGHT_PIN, OUTPUT);

    digitalWrite(RED_LIGHT_PIN, true);
    digitalWrite(GREEN_LIGHT_PIN, false);
    digitalWrite(BLUE_LIGHT_PIN, true);
//    digitalWrite(GROW_LAMP_PIN, false);
    //    update_rtc_time(&NTP, &rtc);
    connect_to_wifi();
    init_ntp(ntp);
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
//    auto value = analogRead(SOIL_MOISTURE_ANALOG_PIN);
//    Serial.println(value);
}