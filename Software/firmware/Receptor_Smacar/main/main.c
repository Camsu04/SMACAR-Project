#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define UART_PORT_NUM      UART_NUM_1
#define UART_BAUD_RATE     9600
#define UART_TX_PIN        GPIO_NUM_17   // TX ESP32 -> RX LoRa
#define UART_RX_PIN        GPIO_NUM_16   // RX ESP32 <- TX LoRa
#define BUF_SIZE           1024

static const char *TAG = "LORA_RX";

void uart_init()
{
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB
    };
    uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_PORT_NUM, &uart_config);
    uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

// Envía comandos AT y lee la respuesta del módulo LoRaWAN
void lorawan_uart_cmd(const char *cmd)
{
    uint8_t data[1024] = {0};
    int result = uart_write_bytes(UART_PORT_NUM, cmd, strlen(cmd));
    if (result > 0)
    {
        ESP_LOGI(TAG, "TX AT: %s", cmd);
        vTaskDelay(pdMS_TO_TICKS(1000));
        int rx = uart_read_bytes(UART_PORT_NUM, data, sizeof(data) - 1, pdMS_TO_TICKS(1000));
        if (rx > 0) {
            data[rx] = '\0';
            ESP_LOGW(TAG, "Respuesta: %s", data);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Error UART TX");
    }
}

void app_main(void)
{
    uart_init();

    // --- Configuración AT LoRaWAN Node ---
    lorawan_uart_cmd("AT\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+LORAMODE=LORA\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+LORAADDR=2\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    //lorawan_uart_cmd("AT+DEVADDR=2\r\n");         // Dirección del receptor (node 2)
    //vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+FREQS=914900000\r\n");   // US915 canal
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+EIRP=22\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+BW=125000\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+SF=12\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+JOIN=1\r\n");
    //lorawan_uart_cmd("AT+MODE=TEST\r\n"); // TEST mode para transparente
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "Receptor UART esperando mensajes del módulo LoRa...");

    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        int len = uart_read_bytes(UART_PORT_NUM, data, BUF_SIZE-1, pdMS_TO_TICKS(5000));
        if (len > 0) {
            data[len] = '\0'; // Null-terminate para printf seguro
            ESP_LOGI(TAG, "Mensaje recibido por UART: %s", data);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
