# Comunicação UART (RX e TX) na Raspberry Pi Pico W com BitDogLab

A comunicação UART (Universal Asynchronous Receiver/Transmitter) é um protocolo serial amplamente utilizado para troca de dados entre microcontroladores e periféricos, como módulos GPS, sensores e outros dispositivos seriais.

## Definições dos sinais RX e TX

- **TX (Transmit)**: é a linha pela qual o dispositivo *envia* dados para outro dispositivo.
- **RX (Receive)**: é a linha pela qual o dispositivo *recebe* dados de outro dispositivo.

Para que a comunicação funcione corretamente, o pino **TX de um dispositivo deve ser conectado ao RX do outro**, e vice-versa.

## Aplicação no projeto com Raspberry Pi Pico W e BitDogLab

Na BitDogLab, que integra a Raspberry Pi Pico W, a conexão UART para módulos externos como o GPS é disponibilizada através de um conector específico, o **conector I2C 0**, que também suporta comunicação UART.

- O conector I2C 0 disponibiliza os pinos:
  - **GPIO0** — funciona como TX da Pico (envia dados para o GPS RX)
  - **GPIO1** — funciona como RX da Pico (recebe dados do GPS TX)
  - Além de 3V3 e GND para alimentação.

Dessa forma, o módulo GPS deve ter seu pino TX conectado ao GPIO1 da Pico (RX), e seu pino RX conectado ao GPIO0 da Pico (TX), respeitando o cruzamento necessário para comunicação UART.

## Configuração no código-fonte (C com SDK Pico)

No código que roda na Raspberry Pi Pico W, os pinos e UART usados para comunicação com o GPS são configurados da seguinte forma:

```c
#define UART_ID uart0       // Usa o hardware UART0 da Pico
#define UART_TX_PIN 0       // GPIO0 da Pico, linha TX (envio para GPS RX)
#define UART_RX_PIN 1       // GPIO1 da Pico, linha RX (recebe do GPS TX)

uart_init(UART_ID, 9600); // Inicializa UART com baudrate 9600, padrão para GPS

gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); // Configura GPIO0 para função UART TX
gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART); // Configura GPIO1 para função UART RX
