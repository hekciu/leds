#include <stdio.h>
#include <stdint.h>

void createDataPackage(uint8_t R, uint8_t G, uint8_t B, int * output) {
    uint8_t colors[3] = { G, R, B };
    int outputBitIndex = 0;

    for (int i = 0; i < 3; i++) {
        uint8_t color = colors[i]; 

        uint8_t colorRes = 0;
        for (int colorBit = 0; colorBit < 8; colorBit++) {
            uint8_t currentMultiplier = (uint8_t)1 << 7 - colorBit;             
            int bit = (color - colorRes) / currentMultiplier;
            *(output + outputBitIndex) = bit;
            colorRes += bit * currentMultiplier;

            outputBitIndex++;
        } 
    }
}
