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
void exp01();
void exp02();
void exp03();
void exp04();

int main(void) {

    board_setup();

    while (1) {
        ks0108_init();

        ks0108_repaint(0);  //проверяем простую запись
        shortDelay(8000000);

        exp01(); //проверяем чтение и перезапись прочитанного запись
        shortDelay(8000000);
        ks0108_paint(0);  //пишем простое число, например, 0 для стирания всего на экране
        shortDelay(8000000);
        exp02(); // спираль из точек
        shortDelay(8000000);
        ks0108_paint(0);
        exp03();
        shortDelay(8000000);
        ks0108_paint(0);
        exp04();
        shortDelay(20000000);
        ks0108_paint(0);
    }
    /* В любых нормальных обстоятельствах мы никогда не попадём сюда */
    return 0;
}

void exp01() {
    uint8_t chip, page, address;
    //set cs
    for (chip = 1; chip < 3; chip++) {
        for (page = 0; page < 8; page++) {
            ks0108_setPage(chip, page);
            ks0108_setAddress(chip, 0);
            uint8_t buffer[64];
            ks0108_receiveData(chip); // ОБЯЗАТЕЛЬНОЕ ФИКТИВНОЕ ЧТЕНИЕ!
            for (address = 0; address < 64; address++) {
                buffer[address] = ks0108_receiveData(chip);
            }
            ks0108_setPage(chip, page);
            ks0108_setAddress(chip, 0);
            for (address = 0; address < 64; address++) {
                uint8_t p = buffer[address];
                p = (p == 0x55) ? 0xaa : 0x55;
                ks0108_sendCmdOrData(chip, 1, 0,  p);
            }
        }
    }
}

void exp02() {
    //вывод попиксельно изображения на экран по спирали по часовой стрелке
    uint8_t xx = 127, yy = 63;
    while (yy>32) {
        uint8_t x = 127-xx, y =63-yy;
        while (x<xx) { // линия по вертикали
            ks0108_drawPixel(x,y, BLACK);
            x+=1;
        }
        while (y<yy) {
            ks0108_drawPixel(x,y, BLACK);
            y+=1;
        }
        while (x>(127-xx)) {
            ks0108_drawPixel(x,y, BLACK);
            x-=1;
        }
        while (y>(63-yy)) {
            ks0108_drawPixel(x,y, BLACK);
            y-=1;
        }
        xx-=1;yy-=1;
    }
}

void exp03() {
    //вывод попиксельно изображения на экран сверху-вниз
    int16_t x, y = 0;
    while (y < 64) {
        x = 127; int8_t d = -1;
        while (x>=0&&x<128) {
            ks0108_drawPixel(x,y,BLACK);
            x+=d;
            if (x==128) {
                d = d * -1;
            }
        }
        y+=1;
    }
    //а теперь слева-направо стираем белыми точками
    x = 0;
    while (x < 128) {
        y = 63; int8_t d = -1;
        while (y>=0&&y<64) {
            ks0108_drawPixel(x,y,WHITE);
            y+=d;
            if (y==64) {
                d = d * -1;
            }
        }
        x+=1;
    }
}

void exp04() {
    int8_t x = 0;
    int8_t y = 64;
    for (;y>0;x+=8, y-=4) {
        ks0108_drawLine(x,63,127,y,BLACK);
    }

    for (x = 0, y=63;y<64;x+=8, y-=4) {
        ks0108_drawLine(x,0,0,y,BLACK);
    }

}

