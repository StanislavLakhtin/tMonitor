#include <libopencm3/stm32/rcc.h>

//#include <stdio.h>
#include <stdlib.h>

#include "atom.h"

#define ONEWIRE_USART3
#define MAXDEVICES_ON_THE_BUS 3

#include "OneWire.h"
#include "ks0108.h"

#define STACK_SIZE      1024
#define THREAD_PRIO     42

static ATOM_TCB main_tcb;

static uint8_t thread_stacks[2][STACK_SIZE];

static void main_thread_func(uint32_t data);
static void paint_pixels_thread_func(uint32_t data);

extern int board_setup(void);

int main(void) {
    int8_t status;
    uint32_t loop;

    /**
     * Brief delay to give the debugger a chance to stop the core before we
     * muck around with the chip's configuration.
     */
    for (loop = 0; loop < 1000000; ++loop) {
        __asm__("nop");
    }

    board_setup();

    /**
     * Initialise OS and set up idle thread
     */
    status = atomOSInit(&thread_stacks[0][0], STACK_SIZE, FALSE);

    if (status == ATOM_OK) {
        /* Set up main thread */
        status = atomThreadCreate(&main_tcb, THREAD_PRIO, paint_pixels_thread_func, 0,
                                  &thread_stacks[1][0], STACK_SIZE, TRUE);
        if (status == ATOM_OK) {
            atomOSStart();
        }
    }

    while (1);

    /* We should never be here */
    return 0;
}


static void paint_pixels_thread_func(uint32_t data __maybe_unused) {
    //ks0108_init();
    while(1) {
        /*int x,y,i, seed;
        for (i = 0; i < 1000; ++ i ) {
            seed = rand();
            x = abs(seed % 128);
            y = abs(seed % 64);
            drawPixel(x, y, WHITE);
        }
        ks0108_repaint();*/
        //gpio_toggle(GPIOA, 0xff);
        atomTimerDelay(SYSTEM_TICKS_PER_SEC);
    }
}
