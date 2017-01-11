//
// Created by Stanislav Lakhtin on 19/11/2016.
//

#include <libopencm3/stm32/rcc.h>

#define ONEWIRE_USART3
#define MAXDEVICES_ON_THE_BUS 3

#include "OneWire.h"
#include "ks0108.h"

extern int board_setup(void);

void shortDelay(uint32_t);

void test01(uint8_t color);

void test02(uint8_t color);

void test03();

void test04(uint8_t color);

void test05();

void test06();

int main(void) {

  board_setup();
  ks0108_init();

  while (1) {
    ks0108_paint(0);
    test02(BLACK); // спираль из точек
    shortDelay(8000000);
    test02(WHITE); // спираль из точек
    shortDelay(8000000);
    test01(BLACK);
    shortDelay(8000000);
    test01(WHITE);
    shortDelay(8000000);
    test04(BLACK);
    shortDelay(8000000);
    test04(WHITE);
    shortDelay(8000000);
    test03();
    shortDelay(8000000);
    test05();
    shortDelay(8000000);
    test06();
    shortDelay(8000000);
  }
  /* В любых нормальных обстоятельствах мы никогда не попадём сюда */
  return 0;
}

void test01(uint8_t color) {
  int16_t x=0, y;
  for (; x < 127; x+=4, y=x/2)
      ks0108_drawLine(x,0,0,(63-y),color);
  for (; x > 0 ; x-=4, y=x/2)
    ks0108_drawLine(127,(63-y),x,63,color);
}

void test02(uint8_t color) {
  //вывод попиксельно изображения на экран по спирали по часовой стрелке
  uint8_t xx = 127, yy = 63;
  while (yy > 32) {
    uint8_t x = 127 - xx, y = 63 - yy;
    while (x < xx) { // линия по вертикали
      ks0108_drawPixel(x, y, color);
      x += 1;
    }
    while (y < yy) {
      ks0108_drawPixel(x, y, color);
      y += 1;
    }
    while (x > (127 - xx)) {
      ks0108_drawPixel(x, y, color);
      x -= 1;
    }
    while (y > (63 - yy)) {
      ks0108_drawPixel(x, y, color);
      y -= 1;
    }
    xx -= 4;
    yy -= 2;
  }
}

void test03() {
  ks0108_paint(0);
  ks0108_drawText(0, 0, BLACK, L"АБВГДЕЁЖЗИКЛМНОПРСТУФХЦЧШЩЫЬЪЭЮЯ");
  ks0108_drawText(0, 8, BLACK, L"ФХЦЧШЩЫЬЪЭЮЯ");
  ks0108_drawText(0, 16, BLACK, L"абвгдеёжзиклмнопрстуфхцчшщыьъэюя");
  ks0108_drawText(0, 24, BLACK, L"чшщыьъэюя-0123456789");
  ks0108_drawText(0, 32, BLACK, L"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  ks0108_drawText(0, 40, BLACK, L"WXYZabcdefjhijklmnopqrstuvwxyz");
  ks0108_drawText(0, 60, BLACK, L"WXYZabcdefjhijklmnopqrstuvwxyz");
}

void test04(uint8_t color) {
  uint8_t radius = 2;
  for (; radius<63;radius+=2)
    ks0108_drawCircle(63,32,radius,color);
}

void test05() {
  ks0108_drawLine(0, 0, 127, 63, BLACK);
  ks0108_drawLine(0, 63, 127, 0, BLACK);
  ks0108_drawLine(0, 0, 127, 0, BLACK);
  ks0108_drawLine(0, 63, 127, 63, BLACK);
  uint16_t i = 0;
  for (; i < 63; i++) {
    ks0108_sendCmdOrData(1, 0, 0, 0xc0 | i);
    ks0108_sendCmdOrData(2, 0, 0, 0xc0 | (63 - i));
    shortDelay(400000);
  }
  ks0108_sendCmdOrData(1, 0, 0, 0xc0);
  ks0108_sendCmdOrData(2, 0, 0, 0xc0);
}

void test06() {
  int16_t x = 0, y = 0, d = 1;
  for (; y>=0&&y<=63;y+=d, x++) {
    ks0108_drawText(x,y,BLACK, L"Привет, мир!");
    shortDelay(500000);
    ks0108_drawText(x,y,WHITE, L"Привет, мир!");
    if (y==63)
      d = -1;
  }
}

