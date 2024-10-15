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



#define T0H_ns 400
#define T1H_ns 800
#define T0L_ns 850
#define T1L_ns 450
#define TReset_us 60

int sendData(timer_group_t timerGroup, timer_idx_t timerId, int gpioPin, int * data, int nsPerTick) {
    int timerCheckpoints[48];
    int transmissionDataLength = 0;
    
    for (int i = 0; i < 48; i = i + 2) {
        int bitValue = *(data + i);
        
        int highTimeTicks = bitValue == 0 ? (T0H_ns / nsPerTick) : (T1H_ns / nsPerTick); 
        int lowTimeTicks = bitValue == 0 ? (T0L_ns / nsPerTick) : (T1L_ns / nsPerTick); 
         
        timerCheckpoints[i] = transmissionDataLength + highTimeTicks; 
        transmissionDataLength += highTimeTicks;
        timerCheckpoints[i + 1] = transmissionDataLength + lowTimeTicks; 
        transmissionDataLength += lowTimeTicks;
        printf("checkpoint: %d\n", timerCheckpoints[i]);
        printf("checkpoint: %d\n", timerCheckpoints[i + 1]);
    }

    uint64_t ticksAtCheckpoint[48];
    // do infinite loop (disable everything) and wait for checkpoints
    int currentCheckpoint = 0;
    uint64_t counterValue = 0;
    portDISABLE_INTERRUPTS();
    timer_start(timerGroup, timerId);
    while (currentCheckpoint < 48) {
        timer_get_counter_value(timerGroup, timerId, &counterValue);
        if (counterValue >= timerCheckpoints[currentCheckpoint]) {
            //sendData
            ticksAtCheckpoint[currentCheckpoint] = counterValue;
            currentCheckpoint++;
        }
    }
    timer_pause(timerGroup, timerId);
    portENABLE_INTERRUPTS();

    for (int i = 0; i < 48; i++) {
        printf("counter at checkpoint number %d, expected value: %d, got: %lld\n", i, timerCheckpoints[i], ticksAtCheckpoint[i]); 
    }

    // wait 60us 
    return -1;
}
