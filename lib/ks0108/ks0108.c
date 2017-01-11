//
// Created by sl on 09.11.16.
//
#include <stdlib.h>
#include "ks0108.h"
#include "ks0108_font.h"

Ks0108Char_t spaceChar = {2, {0x00, 0x00}};

void ks0108_strob();

void ks0108_init() {
  gpio_clear(GPIOA, GPIO_ALL);
  gpio_clear(GPIOB, GPIO0); // RES = 0
  uint16_t delay = 20;
  while (delay--)
      __asm__("nop");
  gpio_set(GPIOB, GPIO0); // RES = 1;
  uint16_t cs;
  for (cs = 2; cs > 0; cs--) {
    ks0108_waitReady(cs, WAITRESETPIN);
    ks0108_sendCmdOrData(cs, 0, 0, 0xc0);
    ks0108_sendCmdOrData(cs, 0, 0, 0x3f);
  }
}

void shortDelay(uint32_t delay) {
  while (delay--)
      __asm__("nop");
}

void ks0108_strob() {
  gpio_set(GPIOA, EPIN);
  __asm__("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
  __asm__("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
  gpio_clear(GPIOA, EPIN);
  __asm__("nop;nop;nop;");
  gpio_clear(GPIOA, GPIO_ALL);
}


void ks0108_CS(uint8_t cs) {
  switch (cs) {
    case 1:
      gpio_set(GPIOA, CHIP1_PIN);
      break;
    case 2:
      gpio_set(GPIOA, CHIP2_PIN);
      break;
  }
}

uint8_t ks0108_receiveData(uint8_t chip) {
  gpio_clear(GPIOC, GPIO13);
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                GPIO_CNF_INPUT_PULL_UPDOWN, 0xff);
  gpio_set(GPIOA, RWPIN | RSPIN);
  ks0108_CS(chip);
  gpio_set(GPIOA, EPIN);
  __asm__("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
  gpio_clear(GPIOA, EPIN);
  shortDelay(40);
  gpio_set(GPIOA, EPIN);
  __asm__("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
  uint8_t data = (uint8_t) gpio_port_read(GPIOA);
  gpio_clear(GPIOA, EPIN);
  __asm__("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, GPIO_ALL);
  gpio_clear(GPIOA, GPIO_ALL);
  return data;
}

void ks0108_waitReady(uint8_t chip, uint16_t waitLines) { //WAITRESETPIN | WAITONOFFPIN | WAITBUSYPIN
  gpio_clear(GPIOA, GPIO_ALL);
  //перевести пины (4,5,7) в состояние входов
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                GPIO_CNF_INPUT_PULL_UPDOWN, waitLines);
  //GPIO_ODR(GPIOA) = !(waitLines);
  ks0108_CS(chip);
  gpio_set(GPIOA, RWPIN);
  gpio_set(GPIOA, EPIN);
  __asm__("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;");
  while ((gpio_port_read(GPIOA) & waitLines)) {
    //uint16_t flags = gpio_port_read(GPIOA);
    uint16_t delay = 20;
    while (delay--)
        __asm__("nop");
  }
  gpio_clear(GPIOA, EPIN);
  __asm__("nop;nop;nop;nop;nop");
  gpio_clear(GPIOA, GPIO_ALL);
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, waitLines);
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
        ks0108_sendCmdOrData(chip, 1, 0, p);
      }
    }
  }
}

void ks0108_paint(uint8_t pattern) {
  uint8_t x, y;
  for (x = 0; x < 128; x += 64)
    for (y = 0; y < 64; y += 8) {
      uint8_t cs = ks0108_GoTo(x, y);
      uint8_t addresses = 64;
      while (addresses--)
        ks0108_sendCmdOrData(cs, 1, 0, pattern);
    }
}

void ks0108_drawCircle(int x, int y, int radius, uint8_t color) {
  if (x < 0 || x > 127 || y < 0 || y > 63)
    return;

  int xx = 0;
  int yy = radius;
  int delta = 1 - 2 * radius;
  int r = 0;
  while (yy >= 0) {
    ks0108_drawPixel(x + xx, y + yy, color);
    ks0108_drawPixel(x + xx, y - yy, color);
    ks0108_drawPixel(x - xx, y + yy, color);
    ks0108_drawPixel(x - xx, y - yy, color);
    r = 2 * (delta + yy) - 1;
    if (delta < 0 && r <= 0) {
      ++xx;
      delta += 2 * xx + 1;
      continue;
    }
    r = 2 * (delta - xx) - 1;
    if (delta > 0 && r > 0) {
      --yy;
      delta += 1 - 2 * yy;
      continue;
    }
    ++xx;
    delta += 2 * (xx - yy);
    --yy;
  }
}

void ks0108_sendCmdOrData(uint8_t cs, uint8_t rs, uint8_t rw, uint8_t data) {
  ks0108_waitReady(cs, WAITBUSYPIN);
  if (cs == 0) {
    //SEND TO BOTH
    gpio_set(GPIOA, CHIP1_PIN);
    gpio_set(GPIOA, CHIP2_PIN);
  } else {
    //send only for one
    ks0108_CS(cs);
  }
  if (rs)
    gpio_set(GPIOA, RSPIN);
  else
    gpio_clear(GPIOA, RSPIN);
  if (rw)
    gpio_set(GPIOA, RWPIN);
  else
    gpio_clear(GPIOA, RWPIN);
  gpio_set(GPIOA, data);
  ks0108_strob();
}

void ks0108_setPage(uint8_t chip, uint8_t page) {
  gpio_clear(GPIOA, GPIO_ALL);
  uint16_t data = (uint16_t) (page | 0x00b8);
  //установить нужную страницу
  ks0108_sendCmdOrData(chip, 0, 0, data);
}

void ks0108_setAddress(uint8_t chip, uint8_t address) {
  gpio_clear(GPIOA, GPIO_ALL);
  uint8_t data = (uint8_t) (address | 0x40);
  //установить нужный адрес
  ks0108_sendCmdOrData(chip, 0, 0, data);
}

Ks0108Char_t *getCharacter(uint16_t s) {
  int i = 0;
  for (; i < charTableSize; ++i) {
    if (charTable[i] == s)
      return chars[i];
  }
  return &spaceChar;
}

uint8_t ks0108_chipByAddress(uint8_t x) { //выбрать номер чипа по адресу
  return (uint8_t) ((x < 64) ? 1 : 2);
}

uint8_t
ks0108_GoTo(uint8_t x, uint8_t y) {//послать команду на позиционирование по адресу, возвращается номер чипа(1 или 2)
  uint8_t cs = ks0108_chipByAddress(x);

  if (cs == 2) {
    x = x - 64;
  }
  uint8_t p = y >> 3;
  ks0108_setPage(cs, p);
  ks0108_setAddress(cs, x);
  return cs;
}

uint8_t ks0108_readMemoryAt(uint8_t x, uint8_t y) {//прочитать данные по адресу
  uint8_t chip = ks0108_chipByAddress(x);
  ks0108_GoTo(x, y);
  return ks0108_receiveData(chip);
}

void
ks0108_drawText(uint8_t x, uint8_t y, uint8_t color, wchar_t *text) { //x и y -- верхний правый угол выводимого текста
  gpio_clear(GPIOA, GPIO_ALL);
  //использовать x и y для настройки на чип, страницу и адрес
  int charPos = 0;
  uint16_t symbol = 0x00;
  do {
    symbol = text[charPos];
    uint8_t curCS = ks0108_chipByAddress(x);
    Ks0108Char_t *charCur = getCharacter(symbol);
    uint8_t cBites = (uint8_t) (y % 8);

    int i = 0;
    for (; i < charCur->size; i++) {
      if (x > 127)
        break;
      uint8_t calcSymLine = ks0108_readMemoryAt(x, y);
      ks0108_GoTo(x, y);
      if (color)
        calcSymLine |= charCur->l[i] << cBites;
      else
        calcSymLine &= ~(charCur->l[i] << cBites);
      ks0108_sendCmdOrData(curCS, 1, 0, calcSymLine);
      if (y < 57 && cBites>0) {
        calcSymLine = ks0108_readMemoryAt(x, y + 8);
        ks0108_GoTo(x, y + 8);
        if (color)
          calcSymLine |= charCur->l[i] >> (8 - cBites);
        else
          calcSymLine &= ~(charCur->l[i] >> (8 - cBites));
        ks0108_sendCmdOrData(curCS, 1, 0, calcSymLine);
      }
      x += 1;
      uint8_t tmpCS = ks0108_chipByAddress(x);
      if (tmpCS != curCS) {
        curCS = tmpCS;
      }
    }

    charPos += 1;
  } while (symbol != 0x00);
}

void ks0108_drawPixel(uint8_t x, uint8_t y, uint8_t color) {
  if ((x > 127) || (y > 63)) return;

  uint8_t chip = ks0108_chipByAddress(x);

  uint8_t current = ks0108_readMemoryAt(x, y);
  uint8_t mask = 1 << (y & 0x07);
  if (color) {
    current |= mask;
  } else {
    current &= ~mask;
  }
  ks0108_setAddress(chip, x);
  ks0108_sendCmdOrData(chip, 1, 0, current);
}

/*void ks0108_drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color) {

  int16_t xMin = x0 < x1 ? x0 : x1;
  int16_t xMax = x0 == xMin ? x1 : x0;

  int16_t yMin = y0 < y1 ? y0 : y1;
  int16_t yMax = y0 == yMin ? y1 : y0;

  int16_t steep = (yMax - yMin) > (xMax - xMin);
  if (steep) {
    _swap_uint16_t(x0, y0);
    _swap_uint16_t(x1, y1);
  }

  if (x0 > x1) {
    _swap_uint16_t(x0, x1);
    _swap_uint16_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = yMax - yMin;

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      ks0108_drawPixel(y0, x0, color);
    } else {
      ks0108_drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}*/

void ks0108_drawLine(int x1, int y1, int x2, int y2, uint8_t color) {
  const int deltaX = abs(x2 - x1);
  const int deltaY = abs(y2 - y1);
  const int signX = x1 < x2 ? 1 : -1;
  const int signY = y1 < y2 ? 1 : -1;
  //
  int error = deltaX - deltaY;
  //
  ks0108_drawPixel(x2, y2, color);
  while (x1 != x2 || y1 != y2) {
    ks0108_drawPixel(x1, y1, color);
    const int error2 = error * 2;
    //
    if (error2 > -deltaY) {
      error -= deltaY;
      x1 += signX;
    }
    if (error2 < deltaX) {
      error += deltaX;
      y1 += signY;
    }
  }

}