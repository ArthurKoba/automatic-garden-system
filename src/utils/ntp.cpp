#include "configs/configs.h"
#include "ntp.h"

void init_ntp(GyverNTP &ntp) {
    Serial.printf("NTP Client Timezone offset: %c%i\n", (TIMEZONE_OFFSET > 0) ? '+' : '\0', TIMEZONE_OFFSET);
    ntp.setHost(NTP_SERVER_DOMAIN);
    ntp.begin(TIMEZONE_OFFSET);
}