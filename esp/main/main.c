#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/timer.h"

#include "_WS2812B.h"

#define OUTPUT_PIN GPIO_NUM_18
#define OUTPUT_REG BIT18
#define GPIO_BIT_MASK (1ULL<<OUTPUT_PIN) 
#define N_LEDS 300

#define STACK_SIZE 4096


void flashLedsTask (void * colorParameter) {
    int * color = (int *) colorParameter;

    for(;;) {
        if(sendData(OUTPUT_REG, color, N_LEDS) == 0) {
            printf("data sent successfully\n");     
        } else {
            fprintf(stderr, "error with sending data\n");
        }; 
        printf("flash led task bitches\n");
        vTaskDelay(100);
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
    
    //xReturned = xTaskCreate(flashLedsTask, "flash_leds_task", STACK_SIZE, (void *) output, configMAX_PRIORITIES - 1, &xHandle);
    xReturned = xTaskCreate(flashLedsTask, "flash_n_leds_task", STACK_SIZE, (void *) output, configMAX_PRIORITIES - 1, &xHandle);
}
