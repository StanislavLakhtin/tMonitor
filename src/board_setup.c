//#include <stdbool.h>
//#include <stdio.h>
//#include <errno.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/systick.h>
//#include <libopencm3/cm3/cortex.h>
//#include <libopencm3/cm3/nvic.h>

//#include "atomport.h"

#ifdef _STDIO_H_

#define USART_CONSOLE USART1

int _write(int file, char *ptr, int len) {
    int i;

    if (file == 1) {
        for (i = 0; i < len; i++)
            usart_send_blocking(USART_CONSOLE, ptr[i]);
        return i;
    }
    errno = EIO;
    return -1;
}

#endif

/**
 * Set up the core clock to something other than the internal 16MHz PIOSC.
 * Make sure that you use the same clock frequency in  systick_setup().
 */
static void clock_setup(void)
{
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);

#ifdef ONEWIRE_USART3
    rcc_periph_clock_enable(RCC_USART3);
#endif
}

static void gpioA(void) {
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, GPIO_ALL);
    gpio_clear(GPIOA, GPIO_ALL);
}

static void gpio_setup(void) {
    gpioA();

    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO_USART3_TX | GPIO_USART3_RX);

    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, GPIO0); //LCD(KS0108) RESET PIN

    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

    gpio_set(GPIOB, GPIO0);
}



/**
 * Callback from your main program to set up the board's hardware before
 * the kernel is started.
 */
int board_setup(void)
{
    /* Disable interrupts. This makes sure that the sys_tick_handler will
     * not be called before the first thread has been started.
     * Interrupts will be enabled by archFirstThreadRestore().
     */
//    cm_mask_interrupts(true);

    /* configure system clock, user LED and UART */
    clock_setup();
    gpio_setup();

    /* initialise SysTick counter */
//    systick_setup();

    /* Set exception priority levels. Make PendSv the lowest priority and
     * SysTick the second to lowest
     */
//    nvic_set_priority(NVIC_PENDSV_IRQ, 0xFF);
//    nvic_set_priority(NVIC_SYSTICK_IRQ, 0xFE);

    return 0;
}

