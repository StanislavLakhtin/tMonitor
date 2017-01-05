//
// Created by sl on 09.11.16.
//
#include "ks0108.h"

void ks0108_init() {
    gpio_clear(GPIOA, GPIO_ALL);
    gpio_clear(GPIOB, GPIO0); // RES = 0
    uint8_t delay = 100;
    while(delay--)
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
        ks0108_send(cmd);
        cmd = DISPLAY_ON;
        cmd.p.cs = (uint8_t) cs;
        ks0108_send(cmd);
    }
}

void ks0108_send(u_PortStruct_t d) {
    gpio_clear(GPIOC, GPIO13); //blink led
    ks0108_waitReady(d.p.cs);
    d.p.e = 0;
    gpio_set(GPIOA, d.raw);
    d.p.e = 1;
    gpio_set(GPIOA, d.raw);
    __asm__("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
    gpio_clear(GPIOA, EPIN);
    __asm__("nop;nop;nop;");
    gpio_clear(GPIOA, GPIO_ALL);
}

uint8_t ks0108_receive(uint8_t chip) {
    gpio_clear(GPIOC, GPIO13);
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_FLOAT, 0xff);
    gpio_set(GPIOA, RWPIN | RSPIN);
    switch (chip) {
        case 1: gpio_set(GPIOA, CHIP1_PIN); break;
        case 2: gpio_set(GPIOA, CHIP2_PIN); break;
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
                  GPIO_CNF_INPUT_FLOAT, WAITRESETPIN | WAITONOFFPIN | WAITBUSYPIN);
    //GPIO_ODR(GPIOA) = !(WAITRESETPIN | WAITONOFFPIN | WAITBUSYPIN);
    switch (chip) {
        case 1: gpio_set(GPIOA, CHIP1_PIN); break;
        case 2: gpio_set(GPIOA, CHIP2_PIN); break;
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
    u_PortStruct_t cmd;
    //set cs
    for (chip = 1; chip < 3; chip++) {
        cmd.p.cs = chip;
        for (page = 0; page < 8; page++) {
            //setpage and address
            cmd.p.db = (uint8_t) (page | 0xb8);
            cmd.p.a0 = 0;
            cmd.p.rw = 0;
            ks0108_send(cmd);
            cmd.p.db = 0x40;
            cmd.p.a0 = 0; cmd.p.rw = 0;
            ks0108_send(cmd);
            uint8_t p = (uint8_t) ((mode == 0) ? 0x55 : 0xaa);
            for (address = 0; address < 64; address++) {
                cmd.p.a0 = 1; cmd.p.rw = 0;
                p = (uint8_t) ((p == 0x55) ? 0xaa : 0x55);
                cmd.p.db = p;
                ks0108_send(cmd);
            }
        }
    }
}

void ks0108_exp01() {
    uint8_t chip, page, address;
    u_PortStruct_t cmd;
    //set cs
    for (chip = 1; chip < 3; chip++) {
        cmd.p.cs = chip;
        for (page = 0; page < 8; page++) {
            //setpage and address
            cmd.p.db = (uint8_t) (page | 0xb8);
            cmd.p.a0 = 0;
            cmd.p.rw = 0;
            ks0108_send(cmd);
            cmd.p.db = 0x40;
            cmd.p.a0 = 0; cmd.p.rw = 0;
            ks0108_send(cmd);
            uint8_t buffer[64];
            for (address = 0; address < 64; address++) {
                buffer[address] = ks0108_receive(chip);
            }
            cmd.p.db = 0x40;
            cmd.p.a0 = 0; cmd.p.rw = 0;
            ks0108_send(cmd);
            for (address = 0; address < 64; address++) {
                cmd.p.a0 = 1; cmd.p.rw = 0;
                uint8_t p = (uint8_t) (buffer[address] == 0x55 ? 0xaa : 0x55);
                cmd.p.db = p;
                ks0108_send(cmd);
            }
        }
    }
}

void ks0108_exp02() {
    uint8_t chip, address;
    u_PortStruct_t cmd;
    //set cs
    for (chip = 1; chip < 3; chip++) {
        cmd.p.cs = chip;
            //setpage and address
            cmd.p.db = (uint8_t) (0xb8);
            cmd.p.a0 = 0;
            cmd.p.rw = 0;
            ks0108_send(cmd);
            cmd.p.db = 0x40;
            cmd.p.a0 = 0; cmd.p.rw = 0;
            ks0108_send(cmd);
            uint8_t b = 0x55;
            for (address = 0; address < 64*8; address++) {
                cmd.p.a0 = 1; cmd.p.rw = 0;
                b = (uint8_t) (b == 0x55 ? 0xaa : 0x55);
                cmd.p.db = b;
                ks0108_send(cmd);
            }

    }
}
