#include "configs/configs.h"

#include <Arduino.h>
#include "utils/wifi.h"
#include "utils/ntp.h"


__attribute__((used)) void setup() {
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    connect_to_wifi();
    init_ntp();
}


__attribute__((used)) void loop() {
    if (NTP.tick()) {
        // вывод даты и времени строкой
        Serial.print(NTP.toString());  // NTP.timeToString(), NTP.dateToString()
        Serial.print(':');
        Serial.println(NTP.ms());  // + миллисекунды текущей секунды. Внутри tick всегда равно 0
    }
    delay(1000);
}