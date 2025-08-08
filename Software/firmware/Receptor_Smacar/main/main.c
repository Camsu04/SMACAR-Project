#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_tls.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"

#define UART_PORT_NUM UART_NUM_1
#define UART_BAUD_RATE 9600
#define UART_TX_PIN GPIO_NUM_17 // TX ESP32 -> RX LoRa
#define UART_RX_PIN GPIO_NUM_16 // RX ESP32 <- TX LoRa
#define BUF_SIZE 1024

#define BLYNK_AUTH_TOKEN "UDCOVVtPTGNn6brczRzDivWzspzKN5jG" // token unico del dashboard
#define WIFI_SSID "Rottweiler" // nombre del wifi
#define WIFI_PASS "Rottweiler051618" // pass del wifi 

// --------Parametros---------
#define TEMP_MIN 05.0
#define TEMP_MAX 25.0
#define EC_MAX 35.00 // (en mS/cm)
#define PH_MIN 6.5
#define PH_MAX 8.5
#define TDS_MAX 500.0 // mg/L

#define OFFLINE_TIMEOUT_MS 1500000 // 15min (ajusta si es necesario)
uint32_t last_data_time = 0;
bool sensores_reportados_offline = false;

static const char *TAG = "LORA_RX";

// Prototipo para evitar warnings
void uart_init(void);

// Simple encoder (solo convierte espacios y paréntesis, puedes ampliarla)
void urlencode(const char *src, char *dest, size_t dest_len)
{
    size_t j = 0;
    for (size_t i = 0; src[i] != 0 && j + 3 < dest_len; ++i)
    {
        unsigned char c = (unsigned char)src[i];
        if (c == ' ')
        {
            dest[j++] = '%';
            dest[j++] = '2';
            dest[j++] = '0';
        }
        else if (c == '(')
        {
            dest[j++] = '%';
            dest[j++] = '2';
            dest[j++] = '8';
        }
        else if (c == ')')
        {
            dest[j++] = '%';
            dest[j++] = '2';
            dest[j++] = '9';
        }
        else if (c == ',')
        {
            dest[j++] = '%';
            dest[j++] = '2';
            dest[j++] = 'C';
        }
        else if (c == '-')
        {
            dest[j++] = '%';
            dest[j++] = '2';
            dest[j++] = 'D';
        }
        else if (c == ':')
        {
            dest[j++] = '%';
            dest[j++] = '3';
            dest[j++] = 'A';
        }
        else
        {
            dest[j++] = c;
        }
    }
    dest[j] = 0;
}

// ----------- FUNCIONES WIFI -----------
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        esp_wifi_connect();
        ESP_LOGI(TAG, "Conectando al WiFi...");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ESP_LOGI(TAG, "WiFi conectado!");
    }
}

void wifi_init_sta(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &wifi_event_handler,
                                        NULL,
                                        &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT,
                                        IP_EVENT_STA_GOT_IP,
                                        &wifi_event_handler,
                                        NULL,
                                        &instance_got_ip);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}

// ----------- ENVÍO A BLYNK -----------
void send_to_blynk(float temperatura, float ec, float ph, float tds)
{
    char url[256];
    esp_http_client_config_t config = {};
    esp_http_client_handle_t client;
    esp_err_t err;

    // Temperatura en V0
    snprintf(url, sizeof(url), "http://blynk.cloud/external/api/update?token=%s&V0=%.2f", BLYNK_AUTH_TOKEN, temperatura);
    config.url = url;
    client = esp_http_client_init(&config);
    err = esp_http_client_perform(client);
    ESP_LOGI("BLYNK", "Send V0 resp=%d", err);
    esp_http_client_cleanup(client);

    // EC en V1
    snprintf(url, sizeof(url), "http://blynk.cloud/external/api/update?token=%s&V1=%.2f", BLYNK_AUTH_TOKEN, ec);
    config.url = url;
    client = esp_http_client_init(&config);
    err = esp_http_client_perform(client);
    ESP_LOGI("BLYNK", "Send V1 resp=%d", err);
    esp_http_client_cleanup(client);

    // pH en V2
    snprintf(url, sizeof(url), "http://blynk.cloud/external/api/update?token=%s&V2=%.2f", BLYNK_AUTH_TOKEN, ph);
    config.url = url;
    client = esp_http_client_init(&config);
    err = esp_http_client_perform(client);
    ESP_LOGI("BLYNK", "Send V2 resp=%d", err);
    esp_http_client_cleanup(client);

    // TDS en V3
    snprintf(url, sizeof(url), "http://blynk.cloud/external/api/update?token=%s&V3=%.2f", BLYNK_AUTH_TOKEN, tds);
    config.url = url;
    client = esp_http_client_init(&config);
    err = esp_http_client_perform(client);
    ESP_LOGI("BLYNK", "Send V3 resp=%d", err);
    esp_http_client_cleanup(client);
}

// Enviar evento a Blynk
void send_blynk_event(const char *event, const char *desc)
{
    char desc_url[128];
    urlencode(desc, desc_url, sizeof(desc_url));
    char url[256];
    snprintf(url, sizeof(url), "http://blynk.cloud/external/api/event/%s?token=%s&desc=%s",
             event, BLYNK_AUTH_TOKEN, desc_url);
    esp_http_client_config_t config = {.url = url};
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

// ----------- UART INIT -----------
void uart_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB};
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
        if (rx > 0)
        {
            data[rx] = '\0';
            ESP_LOGW(TAG, "Respuesta: %s", data);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Error UART TX");
    }
}

// ========== APP MAIN ==========
void app_main(void)
{
    // Inicializa almacenamiento NVS para WiFi
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Conexión WiFi
    wifi_init_sta();
    uart_init();

    // --- Configuración AT LoRaWAN Node ---
    lorawan_uart_cmd("AT\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+LORAMODE=LORA\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+LORAADDR=2\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    // lorawan_uart_cmd("AT+DEVADDR=2\r\n");         // Dirección del receptor (node 2)
    // vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+FREQS=914900000\r\n"); // US915 canal
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+EIRP=22\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+BW=125000\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+SF=12\r\n");
    vTaskDelay(pdMS_TO_TICKS(800));
    lorawan_uart_cmd("AT+JOIN=1\r\n");
    // lorawan_uart_cmd("AT+MODE=TEST\r\n"); // TEST mode para transparente
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "Receptor UART esperando mensajes del módulo LoRa...");

    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

    while (1)
    {
        int len = uart_read_bytes(UART_PORT_NUM, data, BUF_SIZE - 1, pdMS_TO_TICKS(5000));
        if (len > 0)
        {
            data[len] = '\0'; // Null-terminate para printf seguro
            ESP_LOGI(TAG, "Mensaje recibido por UART: %s", data);

            float temperatura = 0, ec = 0, ph = 0, tds = 0;
            char *ptr = strstr((char *)data, "TEMP:");
            if (ptr)
            {
                int res = sscanf(ptr, "TEMP:%fC,EC:%f,pH:%f,TDS:%f", &temperatura, &ec, &ph, &tds);
                if (res == 4)
                {
                    send_to_blynk(temperatura, ec, ph, tds);
                    ESP_LOGI(TAG, "Datos extraídos y enviados a Blynk: T=%.2f, EC=%.2f, pH=%.2f, TDS=%.2f", temperatura, ec, ph, tds);

                    if (temperatura < TEMP_MIN || temperatura > TEMP_MAX)
                        send_blynk_event("temperatura_fuera_de_rango", "Temperatura fuera del rango 20-25C");
                    if (ec > EC_MAX)
                        send_blynk_event("conductividad_fuera_de_rango", "Conductividad fuera de rango max 35 mS/cm");
                    if (ph < PH_MIN || ph > PH_MAX)
                        send_blynk_event("ph_fuera_de_rango", "pH fuera de rango 6.5-8.5");
                    if (tds > TDS_MAX)
                    {
                        ESP_LOGI(TAG, "Enviando evento TDS fuera de rango");
                        send_blynk_event("tds_fuera_de_rango", "TDS fuera de rango max 500 mgL");
                    }

                    // Actualiza última vez de dato válido
                    last_data_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
                    sensores_reportados_offline = false;
                }
                else
                {
                    ESP_LOGW(TAG, "Error extrayendo datos del mensaje: %s", ptr);
                }
            }
            else
            {
                ESP_LOGW(TAG, "No se encontró 'TEMP:' en el mensaje recibido");
            }
        }

        // Chequea si han pasado más de OFFLINE_TIMEOUT_MS ms sin datos válidos
        uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
        if (!sensores_reportados_offline && last_data_time && (now - last_data_time > OFFLINE_TIMEOUT_MS))
        {
            send_blynk_event("sensores_offline", "No se reciben datos de sensores hace 15min");
            sensores_reportados_offline = true;
            ESP_LOGW(TAG, "SENSORES OFFLINE detectado!");
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
