//
// Created by Stanislav Lakhtin on 19/11/2016.
//

#include <libopencm3/stm32/rcc.h>

#include <stdlib.h>

#define ONEWIRE_USART3
#define MAXDEVICES_ON_THE_BUS 3

#include "OneWire.h"
#include "ks0108.h"

extern int board_setup(void);

int main(void) {

    board_setup();
    ks0108_init();

    while (1) {
        gpio_toggle(GPIOC, GPIO13); //blink led

        /*uint16_t x,y, i,seed;
        for (i = 0; i < 10000; ++ i ) {
            seed = rand();
            x = abs(seed % 128);
            y = abs(seed % 64);
            drawPixel(x, y, WHITE);
        }*/
        ks0108_repaint();
    }
    /* В любых нормальных обстоятельствах мы никогда не попадём сюда */
    return 0;
}

