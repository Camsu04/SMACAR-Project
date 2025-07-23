#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "sx126x.h"
#include "onewire.h"

// Definición de pines
#define EC_PIN ADC1_CHANNEL_3   // GPIO4
#define PH_PIN ADC1_CHANNEL_4   // GPIO5
#define TDS_PIN ADC1_CHANNEL_5  // GPIO6
#define DS18B20_PIN GPIO_NUM_0
#define LORA_SDA GPIO_NUM_8
#define LORA_SCL GPIO_NUM_9

// Rangos ideales
#define IDEAL_PH_MIN 6.5
#define IDEAL_PH_MAX 8.5
#define IDEAL_TDS_MAX 500
#define IDEAL_EC_MAX 1000
#define IDEAL_TEMP_MIN 20
#define IDEAL_TEMP_MAX 25

// Variables globales
static const char *TAG = "WaterQuality";
float temperature = 25.0; // Valor inicial, será actualizado por el sensor

// Configuración LoRa
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

// Función para leer ADC
float read_adc(adc1_channel_t channel) {
    int raw = adc1_get_raw(channel);
    return (raw * 5000.0) / 4095.0; // Convertir a mV (ESP32-S3 tiene ADC de 12 bits)
}

// Función para leer pH
float read_ph() {
    float voltage = read_adc(PH_PIN);
    // Calibración del sensor de pH (ajustar según calibración real)
    float ph_value = 7.0 + ((2500.0 - voltage) / 300.0);
    return ph_value;
}

// Función para leer conductividad eléctrica
float read_ec() {
    float voltage = read_adc(EC_PIN);
    // Fórmula simplificada para convertir voltaje a EC (ajustar según calibración)
    float ec_value = voltage * 1000.0 / 200.0; // Aproximación, K=1
    return ec_value;
}

// Función para leer TDS
float read_tds() {
    float voltage = read_adc(TDS_PIN);
    // Compensación por temperatura
    float compensation_coeff = 1.0 + 0.02 * (temperature - 25.0);
    float compensated_voltage = voltage / compensation_coeff;
    // Fórmula para convertir voltaje a TDS
    float tds_value = (133.42 * compensated_voltage*compensated_voltage*compensated_voltage 
                     - 255.86 * compensated_voltage*compensated_voltage 
                     + 857.39 * compensated_voltage) * 0.5;
    return tds_value;
}

// Función para leer temperatura DS18B20
float read_ds18b20() {
    onewire_rom_t rom;
    uint8_t data[9];
    
    onewire_reset(DS18B20_PIN);
    onewire_skip_rom(DS18B20_PIN);
    onewire_write(DS18B20_PIN, 0x44); // Iniciar conversión
    
    vTaskDelay(pdMS_TO_TICKS(750)); // Esperar conversión
    
    onewire_reset(DS18B20_PIN);
    onewire_skip_rom(DS18B20_PIN);
    onewire_write(DS18B20_PIN, 0xBE); // Leer scratchpad
    
    for (int i = 0; i < 9; i++) {
        data[i] = onewire_read(DS18B20_PIN);
    }
    
    int16_t raw_temp = (data[1] << 8) | data[0];
    float temp = raw_temp / 16.0;
    
    return temp;
}

// Función para verificar alertas
void check_alerts(float ph, float tds, float ec, float temp) {
    char alert_msg[256] = {0};
    bool alert = false;
    
    if (ph < IDEAL_PH_MIN || ph > IDEAL_PH_MAX) {
        snprintf(alert_msg + strlen(alert_msg), sizeof(alert_msg) - strlen(alert_msg), 
                "Alerta pH: %.2f (ideal %.1f-%.1f) ", ph, IDEAL_PH_MIN, IDEAL_PH_MAX);
        alert = true;
    }
    
    if (tds > IDEAL_TDS_MAX) {
        snprintf(alert_msg + strlen(alert_msg), sizeof(alert_msg) - strlen(alert_msg), 
                "Alerta TDS: %.0fppm (max %dppm) ", tds, IDEAL_TDS_MAX);
        alert = true;
    }
    
    if (ec > IDEAL_EC_MAX) {
        snprintf(alert_msg + strlen(alert_msg), sizeof(alert_msg) - strlen(alert_msg), 
                "Alerta EC: %.0fuS/cm (max %duS/cm) ", ec, IDEAL_EC_MAX);
        alert = true;
    }
    
    if (temp < IDEAL_TEMP_MIN || temp > IDEAL_TEMP_MAX) {
        snprintf(alert_msg + strlen(alert_msg), sizeof(alert_msg) - strlen(alert_msg), 
                "Alerta Temp: %.1f°C (ideal %d-%d°C) ", temp, IDEAL_TEMP_MIN, IDEAL_TEMP_MAX);
        alert = true;
    }
    
    if (alert) {
        ESP_LOGE(TAG, "%s", alert_msg);
        // Enviar alerta por LoRa
        sx126x_send(&lora, (uint8_t *)alert_msg, strlen(alert_msg), 5000);
    }
}

// Tarea principal de monitoreo
void monitoring_task(void *pvParameters) {
    while (1) {
        // Leer todos los sensores
        temperature = read_ds18b20();
        float ph_value = read_ph();
        float ec_value = read_ec();
        float tds_value = read_tds();
        
        // Verificar alertas
        check_alerts(ph_value, tds_value, ec_value, temperature);
        
        // Imprimir valores
        ESP_LOGI(TAG, "Temp: %.1f°C, pH: %.2f, EC: %.0fuS/cm, TDS: %.0fppm", 
                temperature, ph_value, ec_value, tds_value);
        
        // Enviar datos normales por LoRa
        char lora_msg[128];
        snprintf(lora_msg, sizeof(lora_msg), "T:%.1f|pH:%.2f|EC:%.0f|TDS:%.0f", 
                temperature, ph_value, ec_value, tds_value);
        sx126x_send(&lora, (uint8_t *)lora_msg, strlen(lora_msg), 5000);
        
        vTaskDelay(pdMS_TO_TICKS(10000)); // Esperar 10 segundos
    }
}

void app_main() {
    // Inicializar ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(EC_PIN, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(PH_PIN, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(TDS_PIN, ADC_ATTEN_DB_11);
    
    // Inicializar DS18B20
    onewire_init(DS18B20_PIN);
    
    // Inicializar LoRa
    sx126x_init(&lora, LORA_SDA, LORA_SCL);
    sx126x_set_lora_params(&lora, &lora_params);
    
    // Crear tarea de monitoreo
    xTaskCreate(monitoring_task, "monitoring_task", 4096, NULL, 5, NULL);
}