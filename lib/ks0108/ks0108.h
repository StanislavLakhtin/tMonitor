//
// Created by sl on 09.11.16.
//
#ifndef TMONITOR_KS0108_H
#define TMONITOR_KS0108_H

#include <libopencm3/stm32/gpio.h>

uint8_t buffer[1024];

typedef struct {
    unsigned en:  1;
    unsigned unused: 2; //stm32f103c8t6 bluepill don't use A13-A14
    unsigned a0:  1;
    unsigned rw:  1;
    unsigned res: 1;
    unsigned cs:  2; //E1 or E2 or both
    unsigned db:  8;
} ks0108Port;

union registerGpio {
    uint16_t   data;
    ks0108Port port;
};

#define WHITE 0x0
#define BLACK 0x1

#define EXX 0
#define E1  1
#define E2  2
#define E12 3

//                               en .. a0 rw res cs   DB7-0
#define RESET       (ks0108Port){ 0, 0, 0, 1, 0, E12, 0x00}
#define DISPLAY_ON  (ks0108Port){ 0, 0, 0, 0, 1, E12, 0x3F}
#define DISPLAY_OFF (ks0108Port){ 0, 0, 0, 0, 1, E12, 0x3E}
#define START_LINE  (ks0108Port){ 0, 0, 0, 0, 1, E12, 0xC0}
#define SET_PAGE    (ks0108Port){ 0, 0, 0, 0, 1, EXX, 0xB8}
#define SET_ADDRESS (ks0108Port){ 0, 0, 0, 0, 1, EXX, 0x40}
#define WRITE_RAM   (ks0108Port){ 0, 0, 1, 0, 1, EXX, 0x40}

typedef union registerGpio frame;

void ks0108_init();
void ks0108_send(frame);
void delayMs(uint32_t mks);
frame readPort();
void drawPixel(uint8_t x, uint8_t y, uint8_t color);

void ks0108_repaint();

#endif //TMONITOR_KS0108_H
