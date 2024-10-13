#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"

#include "_WS2812B.h"

#define OUTPUT_PIN GPIO_NUM_18
#define GPIO_BIT_MASK (1ULL<<OUTPUT_PIN) 
#define NUM_LEDS 10

void flash_red() {
    int * output = malloc(24);
    printf("flashin red on pin: %d\n", OUTPUT_PIN);
    createDataPackage(255, 0, 0, output); 

    sendReset();

    for (int i = 0; i < NUM_LEDS; i++) {
        for (int nBit = 0; nBit < 24; nBit++) {
            if (*(output + nBit) == 0) {
                sendZero(); 
            } else {
                sendOne();
            }
        } 
    }

    free(output);
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

    for(;;) {
        vTaskDelay(1000);
        flash_red(); 
    }    
}
