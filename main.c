#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "nvs_flash.h"
#include "esp_netif_ip_addr.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "sdkconfig.h"

// ----------- Configurações de Credenciais e Blynk -----------
// *** ATENÇÃO: Preencha com suas credenciais ***
#define BLYNK_AUTH_TOKEN "N4VDeoCLFBUpWrlwcaYL8beCORPmsU8w" // Seu Auth Token Blynk
#define WIFI_SSID        "Edemilson_Ext"                       // Seu SSID Wi-Fi
#define WIFI_PASS        "6102967710"                     // Sua Senha Wi-Fi
#define TAG              "DRYSHIELD_V2"

// ----------- Definições dos Pinos e Sensores -----------
#define PINO_SENSOR_CHUVA    GPIO_NUM_15
#define PINO_MOTOR_FECHAR    GPIO_NUM_25       // Motor 1 - Fecha a cortina
#define PINO_MOTOR_ABRIR     GPIO_NUM_23       // Motor 2 - Abre a cortina
#define PINO_MOTOR_VENTOINHA GPIO_NUM_26       // Motor 3 - Ventoinha
#define PINO_MOTOR_PTC       GPIO_NUM_27       // Motor PTC

// Definições do acelerômetro MPU6050
#define MPU6050_ADDR 0x68
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MASTER_NUM I2C_NUM_0
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B

// Definições do sensor de tensão
#define ADC_CHANNEL ADC1_CHANNEL_6 // GPIO34
#define ADC_WIDTH   ADC_WIDTH_BIT_12
#define ADC_ATTEN   ADC_ATTEN_DB_11

// ----------- Variáveis de Estado da Lógica de Controle -----------
static bool cortina_esta_fechada = false; // Inicialmente assume-se que a cortina está aberta
static bool motor_fechando = false;
static bool motor_abrindo = false;
static int64_t tempo_inicio_motor = 0;
// *** AJUSTE ESTES VALORES COM BASE NOS SEUS TESTES FÍSICOS ***
static const int64_t DURACAO_MOTOR_US = 5 * 1000 * 1000; // 10 segundos (tempo real de movimento do seu motor)
static const int64_t DELAY_PARA_ABRIR_US = 5 * 1000 * 1000; // 10 segundos de delay após a chuva parar

static bool esperando_para_abrir = false;
static int64_t tempo_parou_de_chover = 0;

// ----------- Eventos Wi-Fi -----------
static EventGroupHandle_t wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

// Variáveis para controlar os logs da ventoinha/PTC para evitar flood
bool ventoinha_ptc_ligados_log_flag = false;
bool ventoinha_ptc_desligados_log_flag = true;

// ----------- Manipulador de Eventos Wi-Fi -----------
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "WIFI_EVENT_STA_START: Conectando ao Wi-Fi...");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED: Wi-Fi desconectado.");
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP_LOGI(TAG, "Tentando reconectar ao Wi-Fi...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP: Conectado! IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// ----------- Conexão Wi-Fi -----------
static void connect_wifi(void) {
    wifi_event_group = xEventGroupCreate();

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .required = false,
                .capable = true
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Configuração de Wi-Fi iniciada. Aguardando conexão...");
}

// ----------- Envia valor float ao Blynk -----------
static void send_blynk_value(const char* pin, float value) {
    char url[256];
    snprintf(url, sizeof(url),
             "http://blynk.cloud/external/api/update?token=%s&%s=%.2f",
             BLYNK_AUTH_TOKEN, pin, value);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .timeout_ms = 5000,
        .disable_auto_redirect = false,
        .buffer_size = 1024,
        .cert_pem = NULL,
        .event_handler = NULL,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        ESP_LOGE(TAG, "Falha ao inicializar HTTP client para float.");
        return;
    }

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        // ESP_LOGI(TAG, "Enviado float para Blynk: %s = %.2f", pin, value); // Comentado para evitar flood de log
    } else {
        ESP_LOGE(TAG, "Erro ao enviar float para Blynk (%s): %s", pin, esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

// ----------- Envia valor string ao Blynk -----------
static void send_blynk_string(const char* pin, const char* value) {
    char url[256];
    snprintf(url, sizeof(url),
             "http://blynk.cloud/external/api/update?token=%s&%s=%s",
             BLYNK_AUTH_TOKEN, pin, value);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .timeout_ms = 5000,
        .disable_auto_redirect = false,
        .buffer_size = 1024,
        .cert_pem = NULL,
        .event_handler = NULL,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        ESP_LOGE(TAG, "Falha ao inicializar HTTP client para string.");
        return;
    }
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        // ESP_LOGI(TAG, "Enviado string para Blynk: %s = %s", pin, value); // Comentado para evitar flood de log
    } else {
        ESP_LOGE(TAG, "Erro ao enviar string para Blynk (%s): %s", pin, esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

// ----------- I2C e Acelerômetro MPU6050 -----------
esp_err_t i2c_master_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        .clk_flags = 0
    };
    esp_err_t res = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "I2C param config falhou (%s)", esp_err_to_name(res));
        return res;
    }
    res = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "I2C driver install falhou (%s)", esp_err_to_name(res));
    }
    return res;
}

void mpu6050_init(void) {
    uint8_t data = 0x00;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, MPU6050_PWR_MGMT_1, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao inicializar o MPU6050 (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "MPU6050 inicializado com sucesso");
    }
}

void mpu6050_read_accel(int16_t *ax, int16_t *ay, int16_t *az) {
    uint8_t data[6];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, MPU6050_ACCEL_XOUT_H, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, sizeof(data), I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erro na leitura do MPU6050 (%s)", esp_err_to_name(ret));
        *ax = *ay = *az = 0;
        return;
    }

    *ax = (int16_t)((data[0] << 8) | data[1]);
    *ay = (int16_t)((data[2] << 8) | data[3]);
    *az = (int16_t)((data[4] << 8) | data[5]);
}

// ----------- Tombamento -----------
bool verificar_tombamento(float angle_x, float angle_y, float threshold) {
    return fabs(angle_x) > threshold || fabs(angle_y) > threshold;
}

// ----------- app_main - Função Principal do Programa -----------
void app_main(void) {
    // --- 1. Inicialização de NVS e Wi-Fi ---
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_LOGW(TAG, "NVS flash erase necessário. Apagando...");
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    connect_wifi();

    // --- 2. Configuração dos Pinos GPIO (Sensores e Motores) ---
    // Sensor de Chuva
    gpio_reset_pin(PINO_SENSOR_CHUVA);
    gpio_set_direction(PINO_SENSOR_CHUVA, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PINO_SENSOR_CHUVA, GPIO_PULLUP_ONLY);
    ESP_LOGI(TAG, "GPIO %d (Sensor Chuva) configurado como INPUT_PULLUP.", PINO_SENSOR_CHUVA);

    // Motores da Cortina
    gpio_reset_pin(PINO_MOTOR_FECHAR);
    gpio_set_direction(PINO_MOTOR_FECHAR, GPIO_MODE_OUTPUT);
    gpio_set_level(PINO_MOTOR_FECHAR, 1); // DESLIGADO/PARADO (assume ativo em LOW para ligar)
    ESP_LOGI(TAG, "GPIO %d (Motor Fechar) configurado como OUTPUT.", PINO_MOTOR_FECHAR);

    gpio_reset_pin(PINO_MOTOR_ABRIR);
    gpio_set_direction(PINO_MOTOR_ABRIR, GPIO_MODE_OUTPUT);
    gpio_set_level(PINO_MOTOR_ABRIR, 1); // DESLIGADO/PARADO (assume ativo em LOW para ligar)
    ESP_LOGI(TAG, "GPIO %d (Motor Abrir) configurado como OUTPUT.", PINO_MOTOR_ABRIR);

    // Ventoinha
    gpio_reset_pin(PINO_MOTOR_VENTOINHA);
    gpio_set_direction(PINO_MOTOR_VENTOINHA, GPIO_MODE_OUTPUT);
    gpio_set_level(PINO_MOTOR_VENTOINHA, 1); // DESLIGADA (assume ativo em LOW para ligar)
    ESP_LOGI(TAG, "GPIO %d (Ventoinha) configurado como OUTPUT.", PINO_MOTOR_VENTOINHA);

    // Motor PTC
    gpio_reset_pin(PINO_MOTOR_PTC);
    gpio_set_direction(PINO_MOTOR_PTC, GPIO_MODE_OUTPUT);
    gpio_set_level(PINO_MOTOR_PTC, 1); // DESLIGADO (assume ativo em LOW para ligar)
    ESP_LOGI(TAG, "GPIO %d (Motor PTC) configurado como OUTPUT.", PINO_MOTOR_PTC);


    // --- 3. Inicialização de I2C e MPU6050 ---
    if (i2c_master_init() == ESP_OK) {
        mpu6050_init();
    } else {
        ESP_LOGE(TAG, "Falha ao inicializar o I2C. MPU6050 não será usado.");
    }

    // --- 4. Inicialização do ADC para Sensor de Tensão ---
    adc1_config_width(ADC_WIDTH);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN);
    ESP_LOGI(TAG, "ADC Canal %d (GPIO %d) configurado.", ADC_CHANNEL, ADC_CHANNEL_6 + 32);

    // --- Variáveis de Leitura de Sensores e Lógica ---
    const float tombamento_threshold = 30.0;
    int16_t ax_raw = 0, ay_raw = 0, az_raw = 0;
    int raw_adc = 0;
    float v_input = 0.0;
    bool tombado = false;

    // As flags de log são globais/estáticas para persistir entre as chamadas do loop
    // Já estão inicializadas fora do app_main

    // Loop principal do programa
    while (1) {
        int64_t tempo_atual = esp_timer_get_time();

        // --- 1. Verificação de Conectividade Wi-Fi ---
        EventBits_t bits = xEventGroupGetBits(wifi_event_group);
        bool is_wifi_connected = (bits & WIFI_CONNECTED_BIT) != 0;

        // Envia status de conexão para Blynk V0
        if (is_wifi_connected) {
            send_blynk_string("V0", "Conectado");
        } else {
            send_blynk_string("V0", "Desconectado");
        }

        // --- 2. Leitura e Lógica do Sensor de Chuva e Controle da Cortina/Ventoinha ---
        bool esta_chovendo = (gpio_get_level(PINO_SENSOR_CHUVA) == 0); // 0 = chuva detectada

        // REGRA 1: FECHAR a cortina se estiver chovendo e não estiver fechando/abrindo
        if (esta_chovendo && !cortina_esta_fechada && !motor_fechando && !motor_abrindo) {
            motor_fechando = true;
            tempo_inicio_motor = tempo_atual;
            gpio_set_level(PINO_MOTOR_FECHAR, 0); // LIGAR MOTOR FECHAR
            ESP_LOGI(TAG, "Chuva detectada! Acionando MOTOR FECHAR (GPIO %d)...", PINO_MOTOR_FECHAR);
        }
        // REGRA 2, PARTE A: Iniciar contagem para ABRIR se parou de chover e cortina está fechada
        else if (!esta_chovendo && cortina_esta_fechada && !esperando_para_abrir && !motor_abrindo) {
            esperando_para_abrir = true;
            tempo_parou_de_chover = tempo_atual;
            ESP_LOGI(TAG, "Chuva parou. Iniciando contagem para abrir cortina (%lld us)...", DELAY_PARA_ABRIR_US);
        }
        // REGRA 2, PARTE B: Acionar motor de ABRIR após o delay
        else if (esperando_para_abrir && !motor_abrindo) {
            if (tempo_atual - tempo_parou_de_chover >= DELAY_PARA_ABRIR_US) {
                motor_abrindo = true;
                tempo_inicio_motor = tempo_atual;
                gpio_set_level(PINO_MOTOR_ABRIR, 0); // LIGAR MOTOR ABRIR
                esperando_para_abrir = false;
                ESP_LOGI(TAG, "Delay para abrir terminou. Acionando MOTOR ABRIR (GPIO %d)...", PINO_MOTOR_ABRIR);
            }
        }
        // REGRA 3: Cancelar abertura se a chuva voltar durante o delay
        else if (esta_chovendo && esperando_para_abrir) {
            esperando_para_abrir = false;
            ESP_LOGI(TAG, "Chuva voltou. Cancelando abertura da cortina.");
        }

        // --- GERENCIAMENTO DO TEMPO DOS MOTORES DA CORTINA ---
        if (motor_fechando) {
            if (tempo_atual - tempo_inicio_motor >= DURACAO_MOTOR_US) {
                gpio_set_level(PINO_MOTOR_FECHAR, 1); // DESLIGAR MOTOR FECHAR
                motor_fechando = false;
                cortina_esta_fechada = true; // CORTINA FECHOU!
                ESP_LOGI(TAG, "Cortina FECHADA. (Variável cortina_esta_fechada = TRUE)"); // Log de depuração
            }
        }
        if (motor_abrindo) {
            if (tempo_atual - tempo_inicio_motor >= DURACAO_MOTOR_US) {
                gpio_set_level(PINO_MOTOR_ABRIR, 1); // DESLIGAR MOTOR ABRIR
                motor_abrindo = false;
                cortina_esta_fechada = false; // CORTINA ABRIU!
                ESP_LOGI(TAG, "Cortina ABERTA. (Variável cortina_esta_fechada = FALSE)"); // Log de depuração
            }
        }
        
        // --- CONTROLE DA VENTOINHA E DO MOTOR PTC ---
        if (cortina_esta_fechada) {
            // Ligar Ventoinha e Motor PTC
            gpio_set_level(PINO_MOTOR_VENTOINHA, 0); // LIGAR VENTOINHA
            gpio_set_level(PINO_MOTOR_PTC, 0);       // LIGAR MOTOR PTC
            
            // Log apenas quando LIGA pela primeira vez neste ciclo de estado
            if (!ventoinha_ptc_ligados_log_flag) {
                ESP_LOGI(TAG, "Ventoinha (GPIO %d) e Motor PTC (GPIO %d): LIGADOS.", PINO_MOTOR_VENTOINHA, PINO_MOTOR_PTC);
                ventoinha_ptc_ligados_log_flag = true;
                ventoinha_ptc_desligados_log_flag = false; // Reset da flag de desligamento
            }
        } else { // Cortina NÃO está fechada (ou seja, está ABERTA ou em movimento)
            // Desligar Ventoinha e Motor PTC
            gpio_set_level(PINO_MOTOR_VENTOINHA, 1); // DESLIGAR VENTOINHA
            gpio_set_level(PINO_MOTOR_PTC, 1);       // DESLIGAR MOTOR PTC
            
            // Log apenas quando DESLIGA pela primeira vez neste ciclo de estado
            if (!ventoinha_ptc_desligados_log_flag) {
                ESP_LOGI(TAG, "Ventoinha (GPIO %d) e Motor PTC (GPIO %d): DESLIGADOS.", PINO_MOTOR_VENTOINHA, PINO_MOTOR_PTC);
                ventoinha_ptc_desligados_log_flag = true;
                ventoinha_ptc_ligados_log_flag = false; // Reset da flag de ligamento
            }
        }

        // --- 3. Leitura de Sensores e Envio para Blynk (somente se Wi-Fi conectado) ---
        if (is_wifi_connected) {
            // Chuva para Blynk (V2)
            int chuva_blynk = esta_chovendo ? 1 : 0;
            send_blynk_value("V2", (float)chuva_blynk);

            // Acelerômetro MPU6050 para Blynk (V1)
            mpu6050_read_accel(&ax_raw, &ay_raw, &az_raw);
            float angle_x = atan2((float)ay_raw, (float)az_raw) * 180.0 / M_PI;
            float angle_y = atan2((float)ax_raw, (float)az_raw) * 180.0 / M_PI;
            tombado = verificar_tombamento(angle_x, angle_y, tombamento_threshold);
            send_blynk_value("V1", (float)tombado);
            if (tombado) {
                ESP_LOGE(TAG, "[ALERTA] Varal Tombado! Ângulo X: %.1f°, Ângulo Y: %.1f°", angle_x, angle_y);
            }

            // Status da Cortina para Blynk (V4)
            send_blynk_value("V4", (float)(cortina_esta_fechada ? 1 : 0));

            // Leitura do Sensor de Tensão (ADC)
            raw_adc = adc1_get_raw(ADC_CHANNEL);
            v_input = (float)raw_adc * 3.3 / 4095.0; // Assumindo V_ref = 3.3V
            send_blynk_value("V5", v_input); // Enviando tensão para V5 no Blynk
        }

        // --- 4. Logs de Status Geral e Atraso ---
        ESP_LOGI(TAG, "[STATUS GERAL] Chuva: %s | Cortina: %s | Ventoinha: %s | Motor PTC: %s | Tensão: %.2f V | Tombado: %s",
                 esta_chovendo ? "SIM" : "NÃO",
                 cortina_esta_fechada ? "FECHADA" : "ABERTA", // AQUI SE MOSTRA O ESTADO REAL DA VARIÁVEL
                 (gpio_get_level(PINO_MOTOR_VENTOINHA) == 0) ? "LIGADA" : "OFF", // AQUI SE MOSTRA O ESTADO DO PINO
                 (gpio_get_level(PINO_MOTOR_PTC) == 0) ? "LIGADO" : "OFF",       // AQUI SE MOSTRA O ESTADO DO PINO
                 v_input,
                 tombado ? "SIM" : "NÃO");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
