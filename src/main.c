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
void exp05();

int main(void) {

    board_setup();
    ks0108_init();

    while (1) {

        //exp01(); //проверяем чтение и перезапись прочитанного запись
        //shortDelay(8000000);
        //ks0108_paint(0);  //пишем простое число, например, 0 для стирания всего на экране
        //shortDelay(8000000);

        exp02(); // спираль из точек
        shortDelay(8000000);
        exp03();
        shortDelay(10000000);
        exp05();
        shortDelay(8000000);
    }
    /* В любых нормальных обстоятельствах мы никогда не попадём сюда */
    return 0;
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
        xx-=4;yy-=2;
    }
}

void exp03() {
    ks0108_paint(0);
    ks0108_sendCmdOrData(1,0,0, 0xc0);
    ks0108_sendCmdOrData(2,0,0, 0xc0);
    ks0108_drawText(0, 0, BLACK, L"АБВГДЕЁЖЗИКЛМНОПРСТУФХЦЧШЩЫЬЪЭЮЯ");
    ks0108_drawText(0, 8, BLACK, L"ФХЦЧШЩЫЬЪЭЮЯ");
    ks0108_drawText(0, 16, BLACK, L"абвгдеёжзиклмнопрстуфхцчшщыьъэюя");
    ks0108_drawText(0, 24, BLACK, L"чшщыьъэюя-0123456789");
    ks0108_drawText(0, 32, BLACK, L"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ks0108_drawText(0, 40, BLACK, L"WXYZabcdefjhijklmnopqrstuvwxyz");
}

void exp04() {
}

void exp05() {
    ks0108_drawLine(0,0,127,63,BLACK);
    ks0108_drawLine(0,63,127,0,BLACK);
    ks0108_drawLine(0,0,127,0, BLACK);
    ks0108_drawLine(0,63,127,63, BLACK);
    uint16_t i=0;
    for (; i<63;i++){
        ks0108_sendCmdOrData(1,0,0, 0xc0|i);
        ks0108_sendCmdOrData(2,0,0, 0xc0|(63-i));
        shortDelay(400000);
    }
    ks0108_sendCmdOrData(1,0,0, 0xc0);
    ks0108_sendCmdOrData(2,0,0, 0xc0);
}

