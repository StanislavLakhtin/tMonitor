//
// Created by sl on 09.11.16.
//
#include "ks0108.h"

void ks0108_init() {
    uint16_t delay, t;
    gpio_clear(GPIOA, GPIO_ALL);
    gpio_clear(GPIOB, GPIO0); // RES = 0
    delay = 100;
    while (delay--)
            __asm__("nop");
    gpio_set(GPIOB, GPIO0); // RES = 1
    gpio_clear(GPIOA, GPIO_ALL);
    delay = 200;
    while (delay--)
            __asm__("nop");
    u_PortStruct_t cmd;
    for (t = 1; t < 3; t++) {
        cmd = START_LINE;
        cmd.p.chip = (uint8_t) t;
        ks0108_send(cmd);
        delay = 80;
        while (delay--)
                __asm__("nop");
        cmd = DISPLAY_ON;
        cmd.p.chip = (uint8_t) t;
        ks0108_send(cmd);
        delay = 80;
        while (delay--)
                __asm__("nop");
    }
}

/* sleep for delay milliseconds */
void delayMs(uint32_t mks) {
    uint32_t delay = 2048 * mks; //todo переделать потом от частоты и таймеры
    while (delay--)
            __asm__("nop");
}

void ks0108_send(u_PortStruct_t d) {
    d.p.e = 0;
    gpio_port_write(GPIOA, d.raw);
    d.p.e = 1;
    gpio_port_write(GPIOA, d.raw);
    __asm__("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
    d.p.e = 0;
    gpio_port_write(GPIOA, d.raw);
    gpio_clear(GPIOA, GPIO_ALL);
}

void drawPixel(uint8_t x, uint8_t y, uint8_t color) {
    uint8_t page;
    if (x < 64) {
        page = 0;
        x = 0;
    } else {
        page = 7;
        x = x - 64;
    }
    page = page + y / 8;
    if (color)
        buffer[page * 64 + x] |= (y % 8) << 1;
    else
        buffer[page * 64 + x] &= !(y % 8) << 1;
}

uint16_t ks0108_readStatus(uint8_t chip) {
    //перевести пины (0-8) в состояние входов
    u_PortStruct_t cmd = READSTATUS;
    cmd.p.chip = chip;
    ks0108_send(cmd);
    uint8_t delay = 3;
    while (delay--)
        __asm__("nop");
    return gpio_get(GPIOA, GPIO_ALL);
}

void ks0108_repaint() {
    uint8_t chip, page, address, delay;
    u_PortStruct_t cmd;
    //set chip
    for (chip = 1; chip < 3; chip++) {
        cmd.p.chip = chip;
        delay = 255;
        while (delay--)
                __asm__("nop");
        for (page = 0; page < 8; page++) {
            //setpage and address
            cmd.p.db = (uint8_t) (page | 0xb8);
            cmd.p.a0 = 0;
            cmd.p.rw = 0;
            ks0108_send(cmd);
            delay = 50;
            while (delay--)
                    __asm__("nop");
            cmd.p.db = 0x40;
            cmd.p.a0 = 0; cmd.p.rw = 0;
            ks0108_send(cmd);
            delay = 50;
            while (delay--)
                    __asm__("nop");
            uint8_t data;
            for (address = 0; address < 64; address++) {
                cmd.p.a0 = 1; cmd.p.rw = 0;
                data = (uint8_t) (data == 0x55 ? 0xaa : 0x55);
                cmd.p.db = data;
                ks0108_send(cmd);
            }
            delay = 50;
            while (delay--)
                    __asm__("nop");
        }
    }
}

