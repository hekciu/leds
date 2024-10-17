#include <stdio.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <rom/ets_sys.h>
#include "esp_attr.h"
#include "hal/cpu_hal.h"
#include "esp_clk_tree.h"


#define T0H_ns 400
#define T1H_ns 800
#define T0L_ns 850
#define T1L_ns 450
#define TReset_us 60

static portMUX_TYPE _spinlock = portMUX_INITIALIZER_UNLOCKED;


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



int IRAM_ATTR sendData(int gpioPin, int * data) {
    uint32_t cpuFreq;
    if(esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_CPU, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &cpuFreq) != 0) {
        fprintf(stderr, "Could not get cpu frequency, unable to send data!\n");
    };

    printf("CPU frequency: %ld\n", cpuFreq);

    int * timerCheckpoints = malloc(48 * sizeof(int));
    int transmissionDataLength = 0;

    int nsPerCpuTick = (1000*1000*1000)/cpuFreq;
    
    for (int i = 0; i < 48; i = i + 2) {
        int bitValue = *(data + i);
        
        int highTimeTicks = bitValue == 0 ? (T0H_ns / nsPerCpuTick) : (T1H_ns / nsPerCpuTick); 
        int lowTimeTicks = bitValue == 0 ? (T0L_ns / nsPerCpuTick) : (T1L_ns / nsPerCpuTick); 
         
        *(timerCheckpoints + i) = transmissionDataLength + highTimeTicks; 
        transmissionDataLength += highTimeTicks;
        *(timerCheckpoints + i + 1) = transmissionDataLength + lowTimeTicks; 
        transmissionDataLength += lowTimeTicks;
    }

    uint64_t ticksAtCheckpoint[48];
    // do infinite loop (disable everything) and wait for checkpoints
    int currentCheckpoint = 0;
    portDISABLE_INTERRUPTS();
    taskENTER_CRITICAL(&_spinlock);
    vTaskSuspendAll();
    uint64_t startCycleCount = cpu_hal_get_cycle_count(); 
    while (currentCheckpoint < 48) {
        if (cpu_hal_get_cycle_count() > startCycleCount + *(timerCheckpoints + currentCheckpoint)) {
            //sendData
            ticksAtCheckpoint[currentCheckpoint] = cpu_hal_get_cycle_count() - startCycleCount;
            currentCheckpoint++;
        }
    }
    //timer_pause(timerGroup, timerId);
    xTaskResumeAll();
    taskEXIT_CRITICAL(&_spinlock);
    portENABLE_INTERRUPTS();

    for (int i = 0; i < 48; i++) {
        printf("counter at checkpoint number %d, expected value: %d, got: %lld\n", i, timerCheckpoints[i], ticksAtCheckpoint[i]); 
    }

    // wait 60us 
    return -1;
}
