#include "tinygps.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static long parse_degrees(const char *term) {
    char deg[3] = {0};
    strncpy(deg, term, 2);
    long degrees = atol(deg);
    float minutes = atof(term + 2);
    return (long)((degrees + minutes / 60.0) * 1000000);
}

void gps_init(TinyGPS* gps) {
    memset(gps, 0, sizeof(TinyGPS));
    gps->latitude = GPS_INVALID_ANGLE;
    gps->longitude = GPS_INVALID_ANGLE;
    gps->altitude = GPS_INVALID_ALTITUDE;
}

bool gps_encode(TinyGPS* gps, char c) {
    if (c == '$') {
        gps->buffer_pos = 0;
        gps->checksum_calc = 0;
        gps->is_checksum = false;
        return false;
    }

    if (c == '*') {
        gps->buffer[gps->buffer_pos] = '\0';
        gps->is_checksum = true;
        return false;
    }

    if (c == '\n' || c == '\r') {
        if (strncmp(gps->buffer, "GPRMC", 5) == 0) {
            char *fields[12] = {0};
            char *ptr = gps->buffer;
            for (int i = 0; i < 12 && ptr; i++) {
                fields[i] = strsep(&ptr, ",");
            }

            if (fields[2] && fields[2][0] == 'A') {
                if (fields[3] && fields[5]) {
                    gps->latitude = parse_degrees(fields[3]);
                    if (fields[4][0] == 'S') gps->latitude = -gps->latitude;

                    gps->longitude = parse_degrees(fields[5]);
                    if (fields[6][0] == 'W') gps->longitude = -gps->longitude;
                }
            }
        }

        gps->sentence_ready = true;
        return true;
    }

    if (gps->buffer_pos < sizeof(gps->buffer) - 1) {
        gps->buffer[gps->buffer_pos++] = c;
        if (!gps->is_checksum)
            gps->checksum_calc ^= c;
    }

    return false;
}

void gps_get_position(const TinyGPS* gps, int32_t* lat, int32_t* lon, uint32_t* age) {
    if (lat) *lat = gps->latitude;
    if (lon) *lon = gps->longitude;
    if (age) *age = gps->fix_age; // implementar age depois se desejar
}
