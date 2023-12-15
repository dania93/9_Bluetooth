#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "driver/gpio.h"

#include "time.h"
#include "sys/time.h"


#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "BT_MN-41"
#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
#define SPP_SHOW_MODE SPP_SHOW_DATA /*Choose show mode: show data or speed*/

#define RED_LED GPIO_NUM_13
#define GREEN_LED GPIO_NUM_12
#define BLUE_LED GPIO_NUM_14
#define YELLOW_LED GPIO_NUM_27
#define WHITE_LED GPIO_NUM_26


const char *tag = "Bluetooth_control_example";

static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;
static struct timeval time_new, time_old;
static long data_num = 0;

static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;

static void init_led(void)
{
    gpio_reset_pin(RED_LED);
    gpio_set_direction(RED_LED, GPIO_MODE_OUTPUT);
    gpio_reset_pin(GREEN_LED);
    gpio_set_direction(GREEN_LED, GPIO_MODE_OUTPUT);
    gpio_reset_pin(BLUE_LED);
    gpio_set_direction(BLUE_LED, GPIO_MODE_OUTPUT);
    gpio_reset_pin(YELLOW_LED);
    gpio_set_direction(YELLOW_LED, GPIO_MODE_OUTPUT);
    gpio_reset_pin(WHITE_LED);
    gpio_set_direction(WHITE_LED, GPIO_MODE_OUTPUT);

    ESP_LOGI(tag, "Init led completed");  
}

// Функція виводу часу передавання даних
static void print_speed(void)
{
    float time_old_s = time_old.tv_sec + time_old.tv_usec / 1000000.0;
    float time_new_s = time_new.tv_sec + time_new.tv_usec / 1000000.0;
    float time_interval = time_new_s - time_old_s;
    float speed = data_num * 8 / time_interval / 1000.0;
    ESP_LOGI(tag, "speed(%fs ~ %fs): %f kbit/s", time_old_s, time_new_s, speed);
    data_num = 0;
    time_old.tv_sec = time_new.tv_sec;
    time_old.tv_usec = time_new.tv_usec;
}

// функція роботи Bluetooth за подіями
static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event)   // події настають у випадку....
    {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(tag, "ESP_SPP_INIT_EVT"); //  коли SPP ініціалізовано
        esp_spp_start_srv(sec_mask, role_slave, 0, SPP_SERVER_NAME);
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT: // після завершення виявлення SPP
        ESP_LOGI(tag, "ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    case ESP_SPP_OPEN_EVT: // коли підключення клієнта SPP відкрито
        ESP_LOGI(tag, "ESP_SPP_OPEN_EVT");
        break;
    case ESP_SPP_CLOSE_EVT: // коли з'єднання SPP закрито 
        ESP_LOGI(tag, "ESP_SPP_CLOSE_EVT");
        break;
    case ESP_SPP_START_EVT: // коли сервер SPP запускається
        ESP_LOGI(tag, "ESP_SPP_START_EVT");
        esp_bt_dev_set_device_name(EXAMPLE_DEVICE_NAME);
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        break;
    case ESP_SPP_CL_INIT_EVT: // коли клієнт SPP ініціював підключення
        ESP_LOGI(tag, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT: // коли підключення SPP отримує дані лише в режимі ESP_SPP_MODE_CB
#if (SPP_SHOW_MODE == SPP_SHOW_DATA)
        esp_log_buffer_hex("", param->data_ind.data, param->data_ind.len);
        for (size_t i = 0; i < (param->data_ind.len)-2; i++)
        {   
            char value = param->data_ind.data[i];
            printf("%c", value);

            switch (value)
            {
            case 'r':
                char massage_1[] = "RED is glowing \n";
                esp_spp_write(param->data_ind.handle, strlen(massage_1), &massage_1);
                gpio_set_level(RED_LED, 1);
                gpio_set_level(GREEN_LED, 0);
                gpio_set_level(BLUE_LED, 0);
                gpio_set_level(YELLOW_LED, 0);
                gpio_set_level(WHITE_LED, 0);
                break;
            case 'g':
                char massage_2[]  = "GREEN is glowing \n";
                esp_spp_write(param->data_ind.handle, strlen(massage_2), &massage_2);
                gpio_set_level(RED_LED, 0);
                gpio_set_level(GREEN_LED, 1);
                gpio_set_level(BLUE_LED, 0);
                gpio_set_level(YELLOW_LED, 0);
                gpio_set_level(WHITE_LED, 0);
                break;
            case 'b':
                char massage_3[] = "BLUE is glowing \n";
                esp_spp_write(param->data_ind.handle, strlen(massage_3), &massage_3);
                gpio_set_level(RED_LED, 0);
                gpio_set_level(GREEN_LED, 0);
                gpio_set_level(BLUE_LED, 1);
                gpio_set_level(YELLOW_LED, 0);
                gpio_set_level(WHITE_LED, 0);
                break;
            case 'y':
                char massage_4[] = "YELLOW is glowing \n";
                esp_spp_write(param->data_ind.handle, strlen(massage_4), &massage_4);
                gpio_set_level(RED_LED, 0);
                gpio_set_level(GREEN_LED, 0);
                gpio_set_level(BLUE_LED, 0);
                gpio_set_level(YELLOW_LED, 1);
                gpio_set_level(WHITE_LED, 0);
                break;
            case 'w':
                char massage_5[] = "WHITE is glowing \n";
                esp_spp_write(param->data_ind.handle, strlen(massage_5), &massage_5);
                gpio_set_level(RED_LED, 0);
                gpio_set_level(GREEN_LED, 0);
                gpio_set_level(BLUE_LED, 0);
                gpio_set_level(YELLOW_LED, 0);
                gpio_set_level(WHITE_LED, 1);
                break; 
            case 'u':
                char massage_6[] = "BLUE and YELLOW is glowing \n";
                esp_spp_write(param->data_ind.handle, strlen(massage_6), &massage_6);
                gpio_set_level(RED_LED, 0);
                gpio_set_level(GREEN_LED, 0);
                gpio_set_level(BLUE_LED, 1);
                gpio_set_level(YELLOW_LED, 1);
                gpio_set_level(WHITE_LED, 0);
                break; 
            case 'a':
                char massage_7[] = "ALL is glowing \n";
                esp_spp_write(param->data_ind.handle, strlen(massage_7), &massage_7);
                gpio_set_level(RED_LED, 1);
                gpio_set_level(GREEN_LED, 1);
                gpio_set_level(BLUE_LED, 1);
                gpio_set_level(YELLOW_LED, 1);
                gpio_set_level(WHITE_LED, 1);
                break; 
            case 'z':
                char massage_8[] = "ALL is OFF \n";
                esp_spp_write(param->data_ind.handle, strlen(massage_8), &massage_8);
                gpio_set_level(RED_LED, 0);
                gpio_set_level(GREEN_LED, 0);
                gpio_set_level(BLUE_LED, 0);
                gpio_set_level(YELLOW_LED, 0);
                gpio_set_level(WHITE_LED, 0);
                break; 
            default:
                break;
            }
        }
        printf("\n");

#else
        gettimeofday(&time_new, NULL);
        data_num += param->data_ind.len;
        if (time_new.tv_sec - time_old.tv_sec >= 3)
        {
            print_speed();
        }
#endif
        break;
    case ESP_SPP_CONG_EVT: // коли статус перевантаженості підключення SPP змінюється лише в режимі ESP_SPP_MODE_CB
        ESP_LOGI(tag, "ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT: // коли операція запису SPP завершується лише в режимі ESP_SPP_MODE_CB
        ESP_LOGI(tag, "ESP_SPP_WRITE_EVT");
        break;
    case ESP_SPP_SRV_OPEN_EVT: // коли підключення до SPP-сервера відкрито 
        ESP_LOGI(tag, "ESP_SPP_SRV_OPEN_EVT");
        gettimeofday(&time_old, NULL);
        break;
    case ESP_SPP_SRV_STOP_EVT: //коли сервер SPP зупиняється
        ESP_LOGI(tag, "ESP_SPP_SRV_STOP_EVT"); 
        break;
    case ESP_SPP_UNINIT_EVT: // коли SPP деініціалізовано
        ESP_LOGI(tag, "ESP_SPP_UNINIT_EVT");
        break;
    default:
        break;
    }
}

// функція встановлення з'єднання між двома пристроями
void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) 
{
    switch (event)
    {
    case ESP_BT_GAP_AUTH_CMPL_EVT: // Подія завершення автентифікації
    {
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGI(tag, "authentication success: %s", param->auth_cmpl.device_name);
            esp_log_buffer_hex(tag, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        }
        else
        {
            ESP_LOGE(tag, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT: // Запит PIN-коду попереднього (старого) підключення
    {
        ESP_LOGI(tag, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit)
        {
            ESP_LOGI(tag, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        }
        else
        {
            ESP_LOGI(tag, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }

#if (CONFIG_BT_SSP_ENABLED == true)  // перевірка чи включений bluetooth

    case ESP_BT_GAP_CFM_REQ_EVT: // запит на підтвердження користувача для з'єднання SSP
        ESP_LOGI(tag, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %ld", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT: // виведення паролю для з'єднання SSP
        ESP_LOGI(tag, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%ld", param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT: // запит пароля на з’єднання SSP
        ESP_LOGI(tag, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif

    case ESP_BT_GAP_MODE_CHG_EVT: // запит на зміну режиму роботи Bluetooth
        ESP_LOGI(tag, "ESP_BT_GAP_MODE_CHG_EVT mode:%d", param->mode_chg.mode);
        break;

    default:
    {
        ESP_LOGI(tag, "event: %d", event);
        break;
    }
    }
    return;
}

void app_main(void)
{
    init_led();  // ініціалізація LED 
    esp_err_t ret = nvs_flash_init(); // ініціалізація енергонезалежної пам'яті
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase()); // очистити флеш пам'ять та провести нову ініціалізацію
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE)); // очищення пам`яті контролера Bluetooth

/* 
*функція esp_bt_controller_mem_release викликається тільки перез ініціалізацією контролера Bluetooth 
* або після його деініціалізації
*/
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT(); // налаштування контролера Bluetooth
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK)
    {
        ESP_LOGE(tag, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) // ввімкнення контролера Bluetooth
    {
        ESP_LOGE(tag, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_init()) != ESP_OK) // виділення ініціалізація та виділення ресурсу під Bluetooth
    {
        ESP_LOGE(tag, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_enable()) != ESP_OK) // ввімкнення самого Bluetooth
    {
        ESP_LOGE(tag, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK)
    {
        ESP_LOGE(tag, "%s gap register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK)
    {
        ESP_LOGE(tag, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_init(esp_spp_mode)) != ESP_OK)
    {
        ESP_LOGE(tag, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);
}
