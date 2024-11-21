#include <stdio.h>
#include <inttypes.h>
#include <string.h>
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


int parse_rgb_string(char * input, uint8_t * r, uint8_t * g, uint8_t * b) {
    uint8_t inputSize = strlen(input);

    const char * EXAMPLE = "0x00,0x00,0x00"; 
    
    if (inputSize != strlen(EXAMPLE)) {
        ESP_LOGE(GATTS_TABLE_TAG, "Parsing rgb string failed, correct format is: 0x00,0x00,0x00");
        return 1;
    }

    char * redString = malloc(5); 
    char * greenString = malloc(5); 
    char * blueString = malloc(5); 
    char * errorString = malloc(strlen(EXAMPLE) + 1);

    memcpy(redString, input, 4);
    *(redString + 4) = '\0';
    memcpy(greenString, input + 5, 4);
    *(greenString + 4) = '\0';
    memcpy(blueString, input + 10, 4);
    *(blueString + 4) = '\0';

    *r = (uint8_t)strtol(redString, &errorString, 16); 
    *g = (uint8_t)strtol(greenString, &errorString, 16); 
    *b = (uint8_t)strtol(blueString, &errorString, 16); 
     

    free(redString);
    free(greenString);
    free(blueString);
    
    if (strlen(errorString) > 0) {
        return 1;
    }

    return 0;
}


void flash_leds_task (void * _) {

    int * color = (int *)_;
    for(;;) {
        char * colorString;
        // define GLOBALLY rgb string length
        esp_gatt_status_t status = esp_ble_gatts_get_attr_value(leds_color_handle_table[IDX_CHAR_VAL_RGB], &RGB_STRING_SIZE, (const uint8_t **)&colorString);

        printf("dupa: %s\n", colorString);
        if(sendData(OUTPUT_REG, color, N_LEDS) == 0) {
            printf("data sent successfully\n");     
        } else {
            fprintf(stderr, "error with sending data\n");
        }; 
        printf("flash led task\n");
        vTaskDelay(FLASH_LEDS_PERIOD_MS);
    }    

    free(color); // do I even have to place it anywhere?
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

    int * output = malloc(24 * sizeof(int));
    printf("flashin red on pin: %d\n", OUTPUT_PIN);
    createDataPackage(255, 255, 255, output); 

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


    xReturned = xTaskCreate(flash_leds_task, "flash_leds_task", STACK_SIZE, (void *) output, configMAX_PRIORITIES - 1, &xHandle);
}
