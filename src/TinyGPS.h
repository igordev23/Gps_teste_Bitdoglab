#ifndef TINYGPS_H
#define TINYGPS_H

#include <stdbool.h>
#include <stdint.h>

#define GPS_INVALID_AGE        0xFFFFFFFF
#define GPS_INVALID_ANGLE      999999999
#define GPS_INVALID_ALTITUDE   999999999
#define GPS_INVALID_DATE       0
#define GPS_INVALID_TIME       0xFFFFFFFF
#define GPS_INVALID_SPEED      999999999
#define GPS_INVALID_SATELLITES 0xFF
#define GPS_INVALID_HDOP       0xFFFFFFFF

typedef struct {
    uint32_t time, date;
    int32_t latitude, longitude;
    uint32_t fix_age;
    int32_t altitude;
    uint16_t sats;
    uint32_t hdop;
    char buffer[100];
    uint8_t buffer_pos;
    bool is_checksum;
    uint8_t checksum_calc;
    uint8_t checksum_read;
    bool sentence_ready;
} TinyGPS;

void gps_init(TinyGPS* gps);
bool gps_encode(TinyGPS* gps, char c);
void gps_get_position(const TinyGPS* gps, int32_t* lat, int32_t* lon, uint32_t* age);

#endif
