//
// Created by sl on 09.11.16.
//
#include "ks0108.h"
#include "ks0108_font.h"

void ks0108_init() {
    gpio_clear(GPIOA, GPIO_ALL);
    gpio_clear(GPIOB, GPIO0); // RES = 0
    uint8_t delay = 100;
    while (delay--)
            __asm__("nop");
    gpio_set(GPIOB, GPIO0); // RES = 1
    ks0108_waitReady(1);
    ks0108_waitReady(2);
    gpio_clear(GPIOA, GPIO_ALL);
    u_PortStruct_t cmd;
    uint16_t cs;
    for (cs = 1; cs < 3; cs++) {
        cmd = START_LINE;
        cmd.p.cs = (uint8_t) cs;
        ks0108_sendCmdOrData(cmd);
        cmd = DISPLAY_ON;
        cmd.p.cs = (uint8_t) cs;
        ks0108_sendCmdOrData(cmd);
    }
}

void shortDelay(uint32_t delay) {
    while (delay--)
            __asm__("nop");
}

void ks0108_sendCmdOrData(u_PortStruct_t d) {
    gpio_clear(GPIOC, GPIO13); //blink led
    //ks0108_waitReady(d.p.cs); не проверяем, а просто делаем паузу -- так быстрее
    shortDelay(40);
    d.p.e = 0;
    gpio_set(GPIOA, d.raw);
    d.p.e = 1;
    gpio_set(GPIOA, d.raw);
    __asm__("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
    gpio_clear(GPIOA, EPIN);
    __asm__("nop;nop;nop;");
    gpio_clear(GPIOA, GPIO_ALL);
}

uint8_t ks0108_receiveData(uint8_t chip) {
    gpio_clear(GPIOC, GPIO13);
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_PULL_UPDOWN, 0xff);
    gpio_set(GPIOA, RWPIN | RSPIN);
    switch (chip) {
        case 1:
            gpio_set(GPIOA, CHIP1_PIN);
            break;
        case 2:
            gpio_set(GPIOA, CHIP2_PIN);
            break;
    }
    __asm__("nop;nop;nop;");
    gpio_set(GPIOA, EPIN);
    __asm__("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
    uint8_t data = (uint8_t) gpio_port_read(GPIOA);
    gpio_clear(GPIOA, EPIN);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, 0xff);
    gpio_clear(GPIOA, GPIO_ALL);
    return data;
}

void ks0108_waitReady(uint8_t chip) {
    gpio_clear(GPIOA, GPIO_ALL);
    //перевести пины (4,5,7) в состояние входов
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_PULL_UPDOWN, WAITRESETPIN | WAITONOFFPIN | WAITBUSYPIN);
    GPIO_ODR(GPIOA) = !(WAITRESETPIN | WAITONOFFPIN | WAITBUSYPIN);
    switch (chip) {
        case 1:
            gpio_set(GPIOA, CHIP1_PIN);
            break;
        case 2:
            gpio_set(GPIOA, CHIP2_PIN);
            break;
    }
    gpio_set(GPIOA, RWPIN);
    gpio_set(GPIOA, EPIN);
    __asm__("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
    while (gpio_port_read(GPIOA) & 0x0090) {
        uint16_t flags = (uint16_t) (gpio_port_read(GPIOA) & 0x00ff);
        if (flags)
            gpio_set(GPIOC, GPIO13); //blink led
    }
    gpio_clear(GPIOA, EPIN);
    __asm__("nop;nop;nop;nop;nop");
    gpio_clear(GPIOA, GPIO_ALL);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, WAITRESETPIN | WAITONOFFPIN | WAITBUSYPIN);
}

void ks0108_repaint(uint8_t mode) {
    uint8_t chip, page, address;
    for (chip = 1; chip < 3; chip++) {
        for (page = 0; page < 8; page++) {
            ks0108_setPage(chip, page);
            ks0108_setAddress(chip, 0);
            uint8_t p = (uint8_t) ((mode == 0) ? 0x55 : 0xaa);
            for (address = 0; address < 64; address++) {
                p = (uint8_t) ((p == 0x55) ? 0xaa : 0x55);
                ks0108_writeData(chip, p);
            }
        }
    }
}

void ks0108_paint(uint8_t pattern) {
    uint8_t chip, page, address;
    for (chip = 1; chip < 3; chip++) {
        for (page = 0; page < 8; page++) {
            ks0108_setPage(chip, page);
            ks0108_setAddress(chip, address);
            for (address = 0; address < 64; address++) {
                ks0108_writeData(chip, pattern);
            }
        }
    }
}

void ks0108_setPage(uint8_t chip, uint8_t page) {
    ks0108_waitReady(chip);
    gpio_clear(GPIOA, GPIO_ALL);
    switch (chip) {
        case 1:
            gpio_set(GPIOA, CHIP1_PIN);
            break;
        case 2:
            gpio_set(GPIOA, CHIP2_PIN);
            break;
    }
    uint16_t data = (uint16_t) (page | 0x00b8);
    //установить нужную страницу
    gpio_set(GPIOA, data&0x00ff);
    gpio_set(GPIOA, EPIN);
    __asm__("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
    gpio_clear(GPIOA, EPIN);
    __asm__("nop;nop;nop;");
    gpio_clear(GPIOA, GPIO_ALL);
}

void ks0108_setAddress(uint8_t chip, uint8_t address) {
    ks0108_waitReady(chip);
    gpio_clear(GPIOA, GPIO_ALL);
    switch (chip) {
        case 1:
            gpio_set(GPIOA, CHIP1_PIN);
            break;
        case 2:
            gpio_set(GPIOA, CHIP2_PIN);
            break;
    }
    uint16_t data = (uint16_t) (address | 0x40);
    //установить нужную страницу
    gpio_set(GPIOA, data&0x00ff);
    gpio_set(GPIOA, EPIN);
    __asm__("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
    gpio_clear(GPIOA, EPIN);
    __asm__("nop;nop;nop;");
    gpio_clear(GPIOA, GPIO_ALL);
}

void ks0108_writeData(uint8_t chip, uint8_t data) {
    ks0108_waitReady(chip);
    gpio_clear(GPIOA, GPIO_ALL);
    switch (chip) {
        case 1:
            gpio_set(GPIOA, CHIP1_PIN);
            break;
        case 2:
            gpio_set(GPIOA, CHIP2_PIN);
            break;
    }
    //установить нужную страницу
    gpio_set(GPIOA, data&0x00ff);
    gpio_set(GPIOA, RSPIN);
    gpio_set(GPIOA, EPIN);
    __asm__("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
    gpio_clear(GPIOA, EPIN);
    __asm__("nop;nop;nop;");
    gpio_clear(GPIOA, GPIO_ALL);
}

void ks0108_drawPixel(uint8_t x, uint8_t y, uint8_t color) {
    if ((x > 127) || (y > 63)) return;
    uint8_t chip;
    if (x < 64) {
        chip = 1;
    } else {
        chip = 2;
        x = (uint8_t) (x - 64);
    }
    uint8_t page = y >> 3;
    ks0108_setPage(chip, page);
    ks0108_setAddress(chip, x);
    ks0108_receiveData(chip);
    uint8_t current = ks0108_receiveData(chip);
    uint8_t mask = 1 << (y & 0x07);
    if (color) {
        current |= mask;
    } else {
        current &= ~mask;
    }
    ks0108_setAddress(chip, x);
    ks0108_writeData(chip, current);
}
