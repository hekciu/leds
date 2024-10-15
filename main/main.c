#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/timer.h"

#include "_WS2812B.h"

#define OUTPUT_PIN GPIO_NUM_18
#define GPIO_BIT_MASK (1ULL<<OUTPUT_PIN) 
#define NUM_LEDS 10

#define TIMER_DIVIDER 2

// I guess??
#define ABP_CLOCK_FREQ_MHZ 80

#define TIMER_FREQ (ABP_CLOCK_FREQ_MHZ / TIMER_DIVIDER)
#define NS_PER_TICK (1000 / TIMER_FREQ)


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

    timer_group_t timerGroup = 0;
    timer_idx_t timerId = 0;
    timer_config_t timerConfig = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = false,
        .alarm_en = false,
        .auto_reload = false,
    }; // default clock source is APB

    printf("Timer real frequency: %d, ns per tick: %d\n", TIMER_FREQ, NS_PER_TICK);

    int * output = malloc(24 * sizeof(int));
    printf("flashin red on pin: %d\n", OUTPUT_PIN);
    createDataPackage(255, 0, 0, output); 

    for(;;) {
        vTaskDelay(1000);
        if(timer_init(timerGroup, timerId, &timerConfig) != ESP_OK) {
            fprintf(stderr, "could not initialize hardware timer with index: %d, in group: %d\n", timerId, timerGroup);         
            continue;
        }

        if(sendData(timerGroup, timerId, OUTPUT_PIN, output, NS_PER_TICK) == 0) {
            printf("data sent successfully\n");     
        } else {
            fprintf(stderr, "error with sending data\n");
        }; 

        timer_deinit(timerGroup, timerId);
    }    

    free(output);
}
