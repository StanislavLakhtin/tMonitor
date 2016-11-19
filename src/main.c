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
    int seed = rand();

    while (1) {
        gpio_toggle(GPIOC, GPIO13);
        /*ks0108_init();

        int x,y,i, seed;
        for (i = 0; i < 1000; ++ i ) {

            x = abs(seed % 128);
            y = abs(seed % 64);
            drawPixel(x, y, WHITE);
        }
        ks0108_repaint();*/
        gpio_toggle(GPIOA, (uint16_t)abs(seed % 0xff));
        delayMs(200);
    }

    /* We should never be here */
    return 0;
}

