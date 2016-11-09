#include <libopencm3/stm32/rcc.h>

//#include <stdio.h>

#include "atom.h"

#define ONEWIRE_USART3
#define MAXDEVICES_ON_THE_BUS 3

#include "OneWire.h"

#define STACK_SIZE      1024
#define THREAD_PRIO     42

static ATOM_TCB main_tcb;

static uint8_t thread_stacks[2][STACK_SIZE];

static void main_thread_func(uint32_t data);



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
        status = atomThreadCreate(&main_tcb, THREAD_PRIO, main_thread_func, 0,
                                  &thread_stacks[1][0], STACK_SIZE, TRUE);

        if (status == ATOM_OK) {
            atomOSStart();
        }
    }

    while (1);

    /* We will never get here */
    return 0;
}

extern void test_led_toggle(void);

static void main_thread_func(uint32_t data __maybe_unused) {
    OneWire ow;
    ow.usart = USART3;
#ifdef _STDIO_H_
    /* Print message */
    printf("Hello, world!\n");
#endif
    /* Loop forever and blink the LED */
    bool readWrite = true;
    uint32_t pDelay = 0, i;

    while (1) {
        if (owResetCmd(&ow) != ONEWIRE_NOBODY) {    // is anybody on the bus?
            owSearchCmd(&ow);                       // take them romId's
            for (i = 0; i < MAXDEVICES_ON_THE_BUS; i++) {
                RomCode *r = &ow.ids[i];
                Temperature t;
                switch (r->family) {
                    case DS18B20:
                        t = readTemperature(&ow, &ow.ids[i],
                                            true); //it will return PREVIOUS value and will send new measure command
#ifdef _STDIO_H_
                    printf("DS18B20 (SN: %x%x%x%x%x%x), Temp: %3d.%d \n", r->code[0], r->code[1], r->code[2],
                           r->code[3], r->code[4], r->code[5], t.inCelsus, t.frac);
#endif
                        break;
                    case DS18S20:
                        t = readTemperature(&ow, &ow.ids[i], true);
#ifdef _STDIO_H_
                    printf("DS18S20 (SN: %x%x%x%x%x%x), Temp: %3d.%d \n", r->code[0], r->code[1], r->code[2],
                           r->code[3], r->code[4], r->code[5], t.inCelsus, t.frac);
#endif
                        break;
                    case 0x00:
                        break;
                    default:
#ifdef _STDIO_H_
                        printf("UNKNOWN Family:%x (SN: %x%x%x%x%x%x)\n", r->family, r->code[0], r->code[1], r->code[2],
                               r->code[3], r->code[4], r->code[5]);
#endif
                        break;
                }
                pDelay = 1200000;
            }
        } else {
            pDelay = 8000000;
        }
        //do something while sensor calculate
        int k = 10;
        while (k > 0) {
            for (i = 0; i < pDelay; i++)    /* Wait a bit. */
                    __asm__("nop");
            k--;
        }
        readWrite = !readWrite;
        atomTimerDelay(SYSTEM_TICKS_PER_SEC);
    }
}
