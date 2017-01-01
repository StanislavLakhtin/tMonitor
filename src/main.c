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
    /*ks0108_init();
    int seed;

    while (1) {
        gpio_toggle(GPIOC, GPIO13); //blink led

        int i;
        for (i=0; i<16; i++) {
            gpio_port_write(GPIOA, 0xffff);
            gpio_clear(GPIOA, 1<<i);
        }

        uint16_t x,y;
        for (i = 0; i < 10000; ++ i ) {
            seed = rand();
            x = abs(seed % 128);
            y = abs(seed % 64);
            drawPixel(x, y, WHITE);
        }
        ks0108_repaint();
        delayMs(2000);
    }*/
    int i = 0, d = 1;
    while (1) {
        if (i > 13 || i < 0 )
            d = -1 * d;
        i+=d;
        uint16_t data = (0x0000) | (1<<i);
        ks0108_send(data);
    }
    /* We should never be here */
    return 0;
}

