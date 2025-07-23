#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sx126x.h"

#define LORA_SDA GPIO_NUM_8
#define LORA_SCL GPIO_NUM_9

static const char *TAG = "LoRaReceiver";

sx126x_t lora;
const sx126x_lora_params_t lora_params = {
    .freq = 914900000,
    .tx_power = 22,
    .spreading_factor = 12,
    .bandwidth = 125000,
    .coding_rate = 5,
    .preamble_len = 8,
    .crc = true,
    .implicit_header = false
};

// Función de callback para recibir datos
void lora_rx_callback(uint8_t *data, uint16_t length, int8_t rssi, int8_t snr) {
    ESP_LOGI(TAG, "Mensaje recibido (RSSI: %d, SNR: %d): %.*s", 
            rssi, snr, length, data);
    
    // Aquí puedes agregar lógica para procesar los datos recibidos
    // como enviarlos a un servidor o mostrarlos en una pantalla
}

void lora_receive_task(void *pvParameters) {
    while (1) {
        uint8_t buffer[256];
        int16_t length = sx126x_receive(&lora, buffer, sizeof(buffer), 60000);
        
        if (length > 0) {
            lora_rx_callback(buffer, length, lora.last_rssi, lora.last_snr);
        }
    }
}

void app_main() {
    // Inicializar LoRa
    sx126x_init(&lora, LORA_SDA, LORA_SCL);
    sx126x_set_lora_params(&lora, &lora_params);
    
    // Configurar en modo recepción continua
    sx126x_set_rx_mode(&lora);
    
    // Crear tarea de recepción
    xTaskCreate(lora_receive_task, "lora_receive_task", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "Receptor LoRa iniciado, esperando mensajes...");
}