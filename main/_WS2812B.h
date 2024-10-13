#include <stdio.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"
#include <rom/ets_sys.h>

/*
    Write blob of 24 bits to output in order that makes is ready for transmitting
*/
void createDataPackage(uint8_t R, uint8_t G, uint8_t B, int * output) {
    uint8_t colors[3] = { G, R, B };
    int outputBitIndex = 0;

    for (int i = 0; i < 3; i++) {
        uint8_t color = colors[i]; 

        uint8_t colorRes = 0;
        for (int colorBit = 0; colorBit < 8; colorBit++) {
            uint8_t currentMultiplier = (uint8_t)1 << (7 - colorBit);             
            int bit = (color - colorRes) / currentMultiplier;
            *(output + outputBitIndex) = bit;
            colorRes += bit * currentMultiplier;

            outputBitIndex++;
        } 
    }
}


/*
    Functions sendOne, sendZero and sendReset are designed to be used inside FreeRTOS's tasks
*/

// TODO: implement faster transition https://www.reddit.com/r/esp32/comments/f529hf/results_comparing_the_speeds_of_different_gpio/?rdt=64373
#define T0H_us 
void sendOne(gpio_num_t pin_num) {
    gpio_set_level(pin_num, 1);
    ets_delay_us(T0H_us);
    gpio_set_level(pin_num, 0);
    ets_delay_us(T0L_us);
}


void sendZero(gpio_num_t pin_num) {
    gpio_set_level(pin_num, 1);
    ets_delay_us(T1H_us);
    gpio_set_level(pin_num, 0);
    ets_delay_us(T1L_us);
}


void sendReset(gpio_num_t pin_num) {
    gpio_set_level(pin_num, 0);
    ets_delay_us(TReset_us);
}
