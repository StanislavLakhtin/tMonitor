//
// Created by sl on 09.11.16.
//
#ifndef TMONITOR_KS0108_H
#define TMONITOR_KS0108_H

#include <libopencm3/stm32/gpio.h>

uint8_t ks0108_buffer[128][2];

union registerGpio {
    uint16_t data;
    struct {
        unsigned unused: 2;
        unsigned en: 1;
        unsigned cmd: 1;
        unsigned rw:1;
        unsigned res: 1;
        unsigned cs: 2;
        unsigned data: 8;
    } ks0108;
};

//#define DISPLAY_ON {0,0,}

typedef union registerGpio frame;

void ks0108_init();
void ks0108_sendByte(frame);
static void delayMs(uint32_t mks);

#endif //TMONITOR_KS0108_H
