//
// Created by sl on 09.11.16.
//
#ifndef TMONITOR_KS0108_H
#define TMONITOR_KS0108_H

#include <libopencm3/stm32/gpio.h>

// uint32_t v=0xdeadc0de;
// printf("g1 %x\n",get_bits(&v,8,10));
#define get_bits(var,offset,length) (((*(var))>>offset)& (1<<length)-1)
#define set_bits(var,offset,length,value) ((*(var)&(0xffffffff ^ ((1<<length)-1)<<offset)) | value<<offset)
// printf("g1 %x\n",set_bits(&v,4,16,0x3133));

uint8_t buffer[1024];

/*
 * (.. A0 ..)  (.. R/W E E2 E1)
 * DB7-DB0 */

/*
 * MT12864J аналог KS0108 компании samsung
 * Выводы распределены следующим образом:
 * 1    -- (+3.3V) питание модуля
 * 2    -- (GND) общая земля
 * 3    -- (U0) вход питания ЖК панели
 * 4-11 -- (DB0-DB7) шина данных
 * 12   -- (E1) выбор кристалла 1
 * 13   -- (E2) выбор кристалла 2
 * 14   -- (RES) сброс (RESET)
 * 15   -- (R/W) чтение/запись
 * 16   -- (A0) команды/данные
 * 17   -- (E) стробирование данных
 * 18   -- (Eee) выход DC-DC преобразователя
 * 19   -- (+LED backlight)
 * 20   -- (-LED backlight)
 *
 * Управление контрастностью осуществляется на основании включения
 * между U0 и U1 переменного сопротивления в районе 10K
 *
 * подключение к stm32f103c8t6(bluepile):
 * (DB7-DB0) -- A7-A0
 * (RES) -- B0
 * (A0) -- A12
 * (R/W) -- A11
 * (E) -- A10
 * (E1) -- A8
 * (E2) -- A9
 *
 *
 */

#define DISPLAY_ON 0x033f
#define START_LINE 0x03A0
#define CLEAR 0xffff

#define WHITE 0xff

void ks0108_init();
void ks0108_send(uint16_t);
void delayMs(uint32_t mks);
void drawPixel(uint8_t x, uint8_t y, uint8_t color);

void ks0108_repaint();

#endif //TMONITOR_KS0108_H
