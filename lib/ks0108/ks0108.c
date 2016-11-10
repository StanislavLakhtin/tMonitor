//
// Created by sl on 09.11.16.
//
#include "ks0108.h"

void ks0108_init() {
    frame f;
    f.ks0108.res = 0;
    ks0108_sendByte(f); //we MUST wait at least 1 mks after that we should wait busy/reset clear in ic or wait 10 mks
    delayMs(1);
    f.ks0108.res = 1;
    ks0108_sendByte(f);
    delayMs(10);
}

/* sleep for delay milliseconds */
static void delayMs(uint32_t mks)
{
    uint32_t t = 0;
    uint32_t delay = 720*mks; //todo переделать потом от частоты и таймеров
    for (t = 0; t < delay; t++)
       __asm__("nop");
}

void ks0108_sendByte(frame f) {
    gpio_set(GPIOA, f.data);
}


