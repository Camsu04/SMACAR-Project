#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "driver/uart.h"
#include "esp_log.h"

// --- DS18B20 OneWire ---
#define DS18B20_GPIO 21

// --- ADC Defines ---
#define ADC_UNIT_ID ADC_UNIT_1
#define EC_ADC_CHANNEL ADC_CHANNEL_3  // GPIO4
#define PH_ADC_CHANNEL ADC_CHANNEL_4  // GPIO5
#define TDS_ADC_CHANNEL ADC_CHANNEL_5 // GPIO6

#define VREF 3300.0
#define ADC_MAX_READING 4095.0

// --- UART Defines  ---
#define LORA_UART_NUM UART_NUM_1
#define LORA_UART_TXD GPIO_NUM_17 // TX del ESP32 al RX del Node
#define LORA_UART_RXD GPIO_NUM_16 // RX del ESP32 al TX del Node
#define LORA_UART_BAUDRATE 9600
#define LORA_UART_BUF_SIZE 1024
#define DEST_ADDR 2
#define SRC_ADRR 1

static const char *TAG = "LORA_TX";

// ----------- Prototipos -----------
float leer_temperatura_ds18b20(void);
static float leer_adc_mV(adc_oneshot_unit_handle_t handle, adc_cali_handle_t cali, adc_channel_t canal);
float calcular_ec(float, float);
float calcular_ph(float, float);
float calcular_tds(float, float);

// ---------- Inicialización UART para LoRaWAN ----------
void lorawan_uart_init()
{
    const uart_config_t uart_config = {
        .baud_rate = LORA_UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB};
    uart_driver_install(LORA_UART_NUM, LORA_UART_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(LORA_UART_NUM, &uart_config);
    uart_set_pin(LORA_UART_NUM, LORA_UART_TXD, LORA_UART_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void lorawan_uart_cmd(const char *cmd)
{
    uint8_t data[1024] = {0};
    int result = uart_write_bytes(LORA_UART_NUM, cmd, strlen(cmd));
    if (result > 0)
    {
        ESP_LOGI(TAG, "Transmision UART Exitosa");
        vTaskDelay(pdMS_TO_TICKS(2000));
        result = uart_read_bytes(LORA_UART_NUM, data, 1024 - 1, pdMS_TO_TICKS(5000));
        if (result >= 0)
        {
            ESP_LOGI(TAG, "RX UART Evento");
            ESP_LOGW(TAG, "%s", data);
        }
        else
        {
            ESP_LOGE(TAG, "Error UART RX");
        }
    }
    else
    {
        ESP_LOGE(TAG, "Error UART TX");
    }
}

// ===================  APP MAIN  ===================
void app_main(void)
{
    // --- Inicialización ADC ---
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = {.unit_id = ADC_UNIT_ID};
    adc_oneshot_new_unit(&init_config, &adc_handle);

    // Configuración de canales
    adc_oneshot_chan_cfg_t chan_config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };

    adc_oneshot_config_channel(adc_handle, EC_ADC_CHANNEL, &chan_config);
    adc_oneshot_config_channel(adc_handle, PH_ADC_CHANNEL, &chan_config);
    adc_oneshot_config_channel(adc_handle, TDS_ADC_CHANNEL, &chan_config);

    // Calibración por canal
    adc_cali_handle_t cali_ec = NULL, cali_ph = NULL, cali_tds = NULL;
    adc_cali_curve_fitting_config_t cali_config_ec = {
        .unit_id = ADC_UNIT_ID,
        .chan = EC_ADC_CHANNEL,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    adc_cali_curve_fitting_config_t cali_config_ph = {
        .unit_id = ADC_UNIT_ID,
        .chan = PH_ADC_CHANNEL,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    adc_cali_curve_fitting_config_t cali_config_tds = {
        .unit_id = ADC_UNIT_ID,
        .chan = TDS_ADC_CHANNEL,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    if (adc_cali_create_scheme_curve_fitting(&cali_config_ec, &cali_ec) != ESP_OK)
    {
        printf("Error calibrando EC\n");
        return;
    }
    if (adc_cali_create_scheme_curve_fitting(&cali_config_ph, &cali_ph) != ESP_OK)
    {
        printf("Error calibrando pH\n");
        return;
    }
    if (adc_cali_create_scheme_curve_fitting(&cali_config_tds, &cali_tds) != ESP_OK)
    {
        printf("Error calibrando TDS\n");
        return;
    }

    // --- Inicializar DS18B20 ---
    gpio_set_direction(DS18B20_GPIO, GPIO_MODE_INPUT_OUTPUT);

    // --- Inicializar UART para LoRaWAN ---
    lorawan_uart_init();
    lorawan_uart_cmd("AT\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    // --- Configurar LoRaWAN Node por UART usando comandos AT ---
    lorawan_uart_cmd("AT+LORAMODE=LORA\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+LORAADDR=1");
    vTaskDelay(pdMS_TO_TICKS(800));
    //lorawan_uart_cmd("AT+DEVADDR=1\r\n"); // Nodo transmisor
    //vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+FREQS=914900000\r\n"); // US915 canal
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+EIRP=22\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+BW=125000\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+SF=12\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    // lorawan_uart_cmd("AT+MODE=TEST\r\n"); // Modo TEST/TRANSPARENT
    //  lorawan_uart_cmd("AT+DESTINATION=2\r\n");       // Si tu módulo lo requiere
    vTaskDelay(pdMS_TO_TICKS(1000)); // Espera tras configurar

    printf("Transmisor LoRaWAN listo en UART. Enviando datos al nodo 2 cada 5s...\n");

    while (1)
    {
        // Leer sensores
        float temperatura = leer_temperatura_ds18b20();
        float voltaje_ec = leer_adc_mV(adc_handle, cali_ec, EC_ADC_CHANNEL);
        float voltaje_ph = leer_adc_mV(adc_handle, cali_ph, PH_ADC_CHANNEL);
        float voltaje_tds = leer_adc_mV(adc_handle, cali_tds, TDS_ADC_CHANNEL);

        float valor_ec = calcular_ec(voltaje_ec, temperatura);
        float valor_ph = calcular_ph(voltaje_ph, temperatura);
        float valor_tds = calcular_tds(voltaje_tds, temperatura);

        char mensaje[90];
        char temp_msj[3];
        char data[55];

        mensaje[0] = '\0';
        data[0] = '\0';

        snprintf(data, sizeof(data), "T:TEMP:%.2fC,EC:%.2f,pH:%.2f,TDS:%04.2f", temperatura, valor_ec, valor_ph, valor_tds);

        for (int i = 0; i < strlen(data); i++)
        {
            sprintf(temp_msj, "%02X", data[i]);
            strcat(mensaje, temp_msj);
        }

        ESP_LOGI(TAG, "%s", mensaje);

        // Envía al nodo 2 con el comando AT+SEND
        char comando[120];
        snprintf(comando, sizeof(comando), "AT+SEND=%s\r\n", mensaje);

        printf("Enviando por LoRa (UART):\n\r %s", comando);
        lorawan_uart_cmd(comando);

        // Lee respuesta del Node
        char response[64];
        int rx_len = uart_read_bytes(LORA_UART_NUM, response, sizeof(response) - 1, pdMS_TO_TICKS(200));
        if (rx_len > 0)
        {
            response[rx_len] = '\0';
            printf("Respuesta Node: %s\n\r", response);
        }

        // Imprimir local
        printf("Voltaje EC: %.2f mV | Voltaje pH: %.2f mV | Voltaje TDS: %.2f mV\n", voltaje_ec, voltaje_ph, voltaje_tds);
        printf("Temp: %.2f °C | EC: %.2f us/cm | pH: %.2f | TDS: %.2f ppm\n",
               temperatura, valor_ec, valor_ph, valor_tds);

        vTaskDelay(pdMS_TO_TICKS(2000)); // Esperar 5 segundos
    }

    // --- Liberar calibración (nunca se ejecuta por el bucle) ---
    adc_cali_delete_scheme_curve_fitting(cali_ec);
    adc_cali_delete_scheme_curve_fitting(cali_ph);
    adc_cali_delete_scheme_curve_fitting(cali_tds);
}
// ------------- ADC Y SENSORES --------------
static float leer_adc_mV(adc_oneshot_unit_handle_t handle, adc_cali_handle_t cali, adc_channel_t canal)
{
    int adc_raw = 0, voltage = 0;
    esp_err_t read_ret = adc_oneshot_read(handle, canal, &adc_raw);
    if (read_ret != ESP_OK)
    {
        printf("Error leyendo ADC canal %d\n", canal);
        return 0;
    }
    esp_err_t cali_ret = adc_cali_raw_to_voltage(cali, adc_raw, &voltage);
    if (cali_ret != ESP_OK)
    {
        printf("Error calibrando ADC canal %d\n", canal);
        return 0;
    }
    return (float)voltage; // en mV
}

// --- FÓRMULAS DE CONVERSIÓN ---
float calcular_ec(float voltaje, float temperatura)
{
    // === REFERENCIAS (a 30°C) ===
    const float REF1_mV = 244.0f;     // en mV
    const float REF1_uS = 1548.0f;    // en µS/cm

    const float REF2_mV = 3100.0f;    // en mV
    const float REF2_uS = 14120.0f;   // en µS/cm

    // === Calcular pendiente y ordenada automáticamente ===
    float m = (REF2_uS - REF1_uS) / (REF2_mV - REF1_mV); // µS/cm por mV
    float b = REF1_uS - m * REF1_mV;                     // µS/cm

    // === Calcular EC cruda a temperatura actual ===
    float ec_raw_uS = m * voltaje + b; // en µS/cm, sin compensar

    // === Compensación de temperatura a 25°C ===
    float factor_comp = 1.0f + 0.0185f * (temperatura - 25.0f);
    float ec25_uS = ec_raw_uS / factor_comp;

    // === Devolver valor compensado a temperatura actual ===
    return ec25_uS * factor_comp; // en µS/cm
}

float calcular_ph(float voltaje, float temperatura)
{
    float voltage = voltaje / 1000.0; // V
    float slope = -5.6548;            // Ajusta según calibración real
    float intercept = 15.509;
    float phValue = slope * voltage + intercept;
    return phValue;
}

float calcular_tds(float voltaje, float temperatura)
{
    // 1) Convertir mV a Voltios
    float voltage_V = voltaje / 1000.0f;

    // 2) Compensación de temperatura
    // Fórmula oficial DFRobot: f(25°C) = f(T) / (1.0 + 0.02 * (T - 25))
    float compensationCoefficient = 1.0f + 0.02f * (temperatura - 25.0f);
    float compensationVoltage = voltage_V / compensationCoefficient;

    // 3) Conversión de voltaje a TDS (ppm) según curva cúbica del fabricante
    // Ecuación: TDS(ppm) = (133.42*V³ - 255.86*V² + 857.39*V) * 0.5
    float tdsValue = (133.42f * compensationVoltage * compensationVoltage * compensationVoltage
                    - 255.86f * compensationVoltage * compensationVoltage
                    + 857.39f * compensationVoltage) * 0.5f;

    // 4) Evitar negativos por ruido
    if (tdsValue < 0) tdsValue = 0;

    return tdsValue; // en ppm
}

// --- Funciones DS18B20 ---
static void delay_us(uint32_t us) { ets_delay_us(us); }

static int ds18b20_reset()
{
    int r;
    gpio_set_direction(DS18B20_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(DS18B20_GPIO, 0);
    delay_us(480);
    gpio_set_direction(DS18B20_GPIO, GPIO_MODE_INPUT);
    delay_us(70);
    r = gpio_get_level(DS18B20_GPIO);
    delay_us(410);
    return r == 0;
}

static void ds18b20_write_bit(int v)
{
    gpio_set_direction(DS18B20_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(DS18B20_GPIO, 0);
    if (v)
    {
        delay_us(10);
        gpio_set_level(DS18B20_GPIO, 1);
        delay_us(55);
    }
    else
    {
        delay_us(65);
        gpio_set_level(DS18B20_GPIO, 1);
        delay_us(5);
    }
}

static int ds18b20_read_bit(void)
{
    int r;
    gpio_set_direction(DS18B20_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(DS18B20_GPIO, 0);
    delay_us(3);
    gpio_set_direction(DS18B20_GPIO, GPIO_MODE_INPUT);
    delay_us(10);
    r = gpio_get_level(DS18B20_GPIO);
    delay_us(53);
    return r;
}

static void ds18b20_write_byte(int data)
{
    for (int i = 0; i < 8; i++)
    {
        ds18b20_write_bit(data & 1);
        data >>= 1;
    }
}

static int ds18b20_read_byte(void)
{
    int data = 0;
    for (int i = 0; i < 8; i++)
    {
        data >>= 1;
        if (ds18b20_read_bit())
        {
            data |= 0x80;
        }
    }
    return data;
}

float leer_temperatura_ds18b20(void)
{
    if (!ds18b20_reset())
    {
        printf("DS18B20 no detectado\n");
        return 25.0;
    }
    ds18b20_write_byte(0xCC); // SKIP ROM
    ds18b20_write_byte(0x44); // CONVERT T
    vTaskDelay(pdMS_TO_TICKS(750));
    if (!ds18b20_reset())
    {
        printf("DS18B20 no detectado post-conv\n");
        return 25.0;
    }
    ds18b20_write_byte(0xCC);
    ds18b20_write_byte(0xBE);
    int temp_lsb = ds18b20_read_byte();
    int temp_msb = ds18b20_read_byte();
    int16_t temp = ((temp_msb << 8) | temp_lsb);
    return (float)temp / 16.0;
}