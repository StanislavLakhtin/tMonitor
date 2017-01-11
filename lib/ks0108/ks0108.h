//
// Created by sl on 09.11.16.
//
#ifndef TMONITOR_KS0108
#define TMONITOR_KS0108

#include <libopencm3/stm32/gpio.h>
#include <wchar.h>


// uint32_t v=0xdeadc0de;
// printf("g1 %x\n",get_bits(&v,8,10));
#define get_bits(var, offset, length) (((*(var))>>offset)& (1<<length)-1)
#define set_bits(var, offset, length, value) ((*(var)&(0xffffffff ^ ((1<<length)-1)<<offset)) | value<<offset)
// printf("g1 %x\n",set_bits(&v,4,16,0x3133));

#ifndef _swap_uint16_t
#define _swap_uint16_t(a, b) { uint16_t t = a; a = b; b = t; }
#endif

typedef struct _PortStruct {
  uint8_t db: 8;
  uint8_t cs:2;
  uint8_t e:1;
  uint8_t rw: 1;
  uint8_t a0: 1;
  uint8_t unused0: 3;
} PortStruct_t;

typedef union _u_PortStruct {
  PortStruct_t p;
  uint16_t raw;
} u_PortStruct_t;

/*
 * (.. A0 .. ..) (R/W E E2 E1)
 * (DB7-DB0) */

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
 * (A0/RS) -- A12
 * (R/W) -- A11
 * (E) -- A10
 * (E1) -- A8
 * (E2) -- A9
 *
 */

//static const u_PortStruct_t DISPLAY_ON = { {0x3f,0,0,0,0,0} };
//static const u_PortStruct_t START_LINE = { {0xc0,0,0,0,0,0} };

#define WHITE 0x00
#define BLACK 0xff

#define RWPIN GPIO11
#define EPIN  GPIO10
#define RSPIN  GPIO12

#define WAITBUSYPIN GPIO7
#define WAITONOFFPIN GPIO5
#define WAITRESETPIN GPIO4
#define CHIP1_PIN GPIO8
#define CHIP2_PIN GPIO9

void ks0108_init();

void ks0108_CS(uint8_t);

void ks0108_sendCmdOrData(uint8_t cs, uint8_t rs, uint8_t rw, uint8_t data);

void delayMs(uint32_t mks);

void ks0108_waitReady(uint8_t chip, uint16_t waitLines);

void ks0108_setPage(uint8_t, uint8_t);

void ks0108_setAddress(uint8_t, uint8_t);

uint8_t ks0108_readMemoryAt(uint8_t x, uint8_t y);

uint8_t ks0108_receiveData(uint8_t);

void ks0108_drawPixel(uint8_t x, uint8_t y, uint8_t color);

uint8_t ks0108_GoTo(uint8_t x, uint8_t y);

void ks0108_drawLine(int x0, int y0, int x1, int y1, uint8_t color);

void ks0108_drawCircle(int x, int y, int radius, uint8_t color);

void ks0108_drawText(uint8_t x, uint8_t y, uint8_t color, wchar_t *text);

void ks0108_repaint(uint8_t mode);

void ks0108_paint(uint8_t pattern);

#endif //TMONITOR_KS0108
