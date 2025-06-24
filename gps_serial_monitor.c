#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

#define UART_ID uart1
#define BAUD_RATE 9600

// Definições dos pinos UART
#define UART_TX_PIN 0 // Pino de transmissão (TX)
#define UART_RX_PIN 1 // Pino de recepção (RX)

char nmea_sentence[100];
int idx = 0;

// Timeout para detectar ausência de dados em microssegundos
#define GPS_TIMEOUT_US (5 * 1000 * 1000)  // 5 segundos

uint64_t last_data_time = 0;

void parse_gpgga(const char *nmea) {
    // Copia a string para não modificar o original
    char buffer[100];
    strncpy(buffer, nmea, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    // Divide os campos
    char *fields[15];
    int field_count = 0;
    char *token = strtok(buffer, ",");

    while (token && field_count < 15) {
        fields[field_count++] = token;
        token = strtok(NULL, ",");
    }

    if (field_count >= 10) {
        // Latitude
        float raw_lat = atof(fields[2]);
        char lat_dir = fields[3][0];
        float lat_deg = (int)(raw_lat / 100);
        float lat_min = raw_lat - (lat_deg * 100);
        float latitude = lat_deg + lat_min / 60.0;
        if (lat_dir == 'S') latitude *= -1;

        // Longitude
        float raw_lon = atof(fields[4]);
        char lon_dir = fields[5][0];
        float lon_deg = (int)(raw_lon / 100);
        float lon_min = raw_lon - (lon_deg * 100);
        float longitude = lon_deg + lon_min / 60.0;
        if (lon_dir == 'W') longitude *= -1;

        // Satélites
        int satellites = atoi(fields[7]);

        // Exibe
        printf("Latitude: %.6f\n", latitude);
        printf("Longitude: %.6f\n", longitude);
        printf("Satelites: %d\n", satellites);
        printf("-------------\n");
    }
}

void process_nmea_line(const char *nmea) {
    if (strncmp(nmea, "$GPGGA", 6) == 0) {
        parse_gpgga(nmea);
    }
}

int main() {
    stdio_init_all();

    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    printf("Esperando dados do GPS...\n");

    last_data_time = time_us_64();

    while (true) {
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            last_data_time = time_us_64();  // atualiza tempo do último dado recebido

            if (c == '\n') {
                nmea_sentence[idx] = '\0';
                process_nmea_line(nmea_sentence);
                idx = 0;
            } else if (c != '\r' && idx < sizeof(nmea_sentence) - 1) {
                nmea_sentence[idx++] = c;
            }
        } else {
            // Sem dado no momento, verifica timeout
            uint64_t now = time_us_64();
            if (now - last_data_time > GPS_TIMEOUT_US) {
                printf(">> Aviso: Nenhum dado GPS detectado nos ultimos 5 segundos! Verifique conexoes.\n");
                last_data_time = now; // evita imprimir várias vezes seguidas
            }
            // Pequeno delay para não travar CPU
            sleep_ms(100);
        }
    }

    return 0;
}
