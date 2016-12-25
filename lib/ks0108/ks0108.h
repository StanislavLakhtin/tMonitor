//
// Created by sl on 09.11.16.
//
#ifndef TMONITOR_KS0108_H
#define TMONITOR_KS0108_H

#include <libopencm3/stm32/gpio.h>

// uint32_t v=0xdeadc0de;
// printf("g1 %x\n",get_bits(&v,8,10));
// #define get_bits(Var,Offset,Len) (((*(Var))>>Offset)& (1<<Len)-1)
// #define set_bits(Var,Offset,Len,Value) ((*(Var)&(0xffffffff ^ ((1<<Len)-1)<<Offset)) | Value<<Offset)
// printf("g1 %x\n",set_bits(&v,4,16,0x3133));

uint8_t buffer[1024];

/*
 * (.. A0 ..)  (.. R/W E E2 E1)
 * DB0-DB7 */

#define DISPLAY_ON 0x033f
#define START_LINE 0x03A0
#define CLEAR 0xffff

void ks0108_init();
void ks0108_send(uint16_t);
void delayMs(uint32_t mks);
void drawPixel(uint8_t x, uint8_t y, uint8_t color);

void ks0108_repaint();

#endif //TMONITOR_KS0108_H
