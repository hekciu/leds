#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "nvs_flash.h"

#include "WS2812B.h"
#include "bluetooth.h"

#define OUTPUT_PIN GPIO_NUM_18
#define OUTPUT_REG BIT18
#define GPIO_BIT_MASK (1ULL<<OUTPUT_PIN) 
#define N_LEDS 300
#define FLASH_LEDS_PERIOD_MS 200

#define STACK_SIZE 4096

#define NVS_RGB_VAR_NAME "RGB_VALUE"
#define NVS_RGB_NAMESPACE "RGB_NAMESPACE"


static long parseLong(const char *str, int * code) {
    errno = 0;
    char *temp;
    long val = strtol(str, &temp, 16);

    if (temp == str || *temp != '\0' || ((val == LONG_MIN || val == LONG_MAX) && errno == ERANGE)) {
        fprintf(stderr, "Could not convert '%s' to long and leftover string is: '%s'\n", str, temp);
        *code = 1;
    }

    return val;
}

int parse_rgb_string(char * input, uint8_t * r, uint8_t * g, uint8_t * b) {
    uint8_t inputSize = strlen(input);

    const char * EXAMPLE = "0x00,0x00,0x00"; 
    const int correctDataSize = strlen(EXAMPLE);
    
    if (inputSize != correctDataSize) {
        ESP_LOGE(GATTS_TABLE_TAG, "Parsing rgb string failed, correct format is: 0x00,0x00,0x00 got: %s\n size: %d vs %d", input, correctDataSize, inputSize);
        return 1;
    }

    char * redString = malloc(5); 
    char * greenString = malloc(5); 
    char * blueString = malloc(5); 
    char * errorString;

    memcpy(redString, input, 4);
    *(redString + 4) = '\0';
    memcpy(greenString, input + 5, 4);
    *(greenString + 4) = '\0';
    memcpy(blueString, input + 10, 4);
    *(blueString + 4) = '\0';

    int code = 0;
    
    *r = (uint8_t)parseLong(redString, &code); 
    *g = (uint8_t)parseLong(greenString, &code);
    *b = (uint8_t)parseLong(blueString, &code);

    free(redString);
    free(greenString);
    free(blueString);

    if (code != 0) {
        return 1;
    }

    return 0;
}


void flash_leds_task(void * _) {
    for(;;) {
        vTaskDelay(FLASH_LEDS_PERIOD_MS);

        char * colorString;

        esp_gatt_status_t status = esp_ble_gatts_get_attr_value(leds_color_handle_table[IDX_CHAR_VAL_RGB], &RGB_STRING_SIZE, (const uint8_t **)&colorString);

        if (status != ESP_OK) {
            ESP_LOGE(GATTS_TABLE_TAG, "esp function esp_ble_gatts_get_attr_value failed with code: %d, skipping flash_leds_task", status);   
            continue;
        }

        uint8_t R, G, B;
        if (parse_rgb_string(colorString, &R, &G, &B) != 0) {
            ESP_LOGE(GATTS_TABLE_TAG, "parse_rgb_string failed, skipping flash_leds_task");
            continue;
        }

        int * color = malloc(24 * sizeof(int));
        createDataPackage(R, G, B, color);

        printf("R: %d, G: %d, B: %d\n", R, G, B);
        if (sendData(OUTPUT_REG, color, N_LEDS) == 0) {
            printf("data sent successfully\n");
        } else {
            fprintf(stderr, "error with sending data\n");
        };

        free(color);

        printf("saving data to nvs...\n");

        nvs_handle_t handle;
        nvs_open(NVS_RGB_NAMESPACE, NVS_READWRITE, &handle);

        if (nvs_set_str(handle, NVS_RGB_VAR_NAME, colorString) == ESP_OK) {
            printf("data saved successfully\n");
        } else {
            fprintf(stderr, "error occured while saving data to nvs :(\n");
        }

        nvs_close(handle);
    }    

}


void app_main(void)
{
    printf("starting\n");
   
    // configure gpio for communication with WS2812B
    gpio_config_t ioConf = {};
    ioConf.intr_type = GPIO_INTR_DISABLE;
    ioConf.mode = GPIO_MODE_OUTPUT;
    ioConf.pull_down_en = 0;
    ioConf.pull_up_en = 0;
    ioConf.pin_bit_mask = GPIO_BIT_MASK;
    gpio_config(&ioConf);

    printf("using pin: %d\n", OUTPUT_PIN);
     
    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;

    esp_err_t ret;

    /* Initialize NVS. */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    /*
        Get the last used color from NVS (if there is one)
    */
    nvs_handle_t handle;

    nvs_open(NVS_RGB_NAMESPACE, NVS_READONLY, &handle);

    // RGB_STRING_SIZE, leds_color_rgb are defined in bluetooth.h
    char * rgbData = strndup((const char *)leds_color_rgb, RGB_STRING_SIZE - 1);
    size_t rgbDataLength = RGB_STRING_SIZE;

    esp_err_t lastValueRet = nvs_get_str(handle, NVS_RGB_VAR_NAME, rgbData, &rgbDataLength);

    if (lastValueRet) {
        printf("did not find previously used rgb value, proceeding with default: %s\n", rgbData);
    } else {
        printf("got last used rgb data: %s\n", rgbData);
        memcpy(leds_color_rgb, rgbData, RGB_STRING_SIZE);
    }

    free(rgbData);

    nvs_close(handle);

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);

    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s init controller failed: %s", __func__, esp_err_to_name(ret));
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret){
        ESP_LOGE(GATTS_TABLE_TAG, "gatts register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        ESP_LOGE(GATTS_TABLE_TAG, "gap register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gatts_app_register(ESP_APP_ID);
    if (ret){
        ESP_LOGE(GATTS_TABLE_TAG, "gatts app register error, error code = %x", ret);
        return;
    }

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret){
        ESP_LOGE(GATTS_TABLE_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }


    xReturned = xTaskCreate(flash_leds_task, "flash_leds_task", STACK_SIZE, NULL, configMAX_PRIORITIES - 1, &xHandle);
}
