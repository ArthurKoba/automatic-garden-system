#include "configs/configs.h"
#include "ntp.h"

void init_ntp() {
    Serial.printf("NTP Client Timezone offset: %c%i\n", (NTP_TIMEZONE > 0) ? '+' : '\0', NTP_TIMEZONE);
    NTP.begin(NTP_TIMEZONE);
    NTP.setHost(NTP_SERVER_DOMAIN);
}