#ifndef AUTOMATIC_GARDEN_SYSTEM_NTP_H
#define AUTOMATIC_GARDEN_SYSTEM_NTP_H

#if not defined(NTP_SERVER_DOMAIN)
#define NTP_SERVER_DOMAIN "pool.ntp.org"
#endif

#if not defined(TIMEZONE_OFFSET)
#define TIMEZONE_OFFSET 0
#endif

#include <GyverNTP.h>

void init_ntp(GyverNTP &ntp);

#endif //AUTOMATIC_GARDEN_SYSTEM_NTP_H
