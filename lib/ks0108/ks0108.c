//
// Created by sl on 09.11.16.
//
#include "ks0108.h"

void ks0108_init() {
    frame f;
    f.port = RESET;
    ks0108_send(f); //we MUST wait at least 1 mks after that we should wait busy/reset clear in ic or wait 10 mks
    do {
        f = readPort();
    } while (f.port.db & 0x90);
    f.port = DISPLAY_ON;
    ks0108_send(f);
    f.port = START_LINE;
    ks0108_send(f);
}

/* sleep for delay milliseconds */
void delayMs(uint32_t mks) {
    uint32_t t = 0;
    uint32_t delay = 720 * mks; //todo переделать потом от частоты и таймеры
    for (t = 0; t < delay; t++)
            __asm__("nop");
}

void ks0108_send(frame f) {
    frame clr;
    clr.port = CLEAR;
    gpio_set(GPIOA, f.data);
    delayMs(1);
    gpio_set(GPIOA, clr.data);
}

frame readPort() {
    frame r;
    gpio_port_read(r.data);
    return r;
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

void ks0108_repaint() {
    uint8_t page = 0, CS, p;
    frame f;
    while (page < 16) {
        if (page < 8 ) {
            CS = E1;
            p = page;
        } else {
            CS = E2;
            p = page - 8;
        }
        f.port = SET_PAGE;
        f.port.cs = CS;
        f.port.db |= p;
        ks0108_send(f);
        f.port = SET_ADDRESS;
        f.port.cs = CS;
        ks0108_send(f);
        uint8_t pos = 0;
        while (pos < 64) {
            uint16_t b = page*64 + pos;
            f.port = WRITE_RAM;
            f.port.cs = CS;
            f.port.db = buffer[b];
            ks0108_send(f);
            pos++;
        }
        page++;
    }
}

