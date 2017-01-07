//
// Created by Stanislav Lakhtin on 19/11/2016.
//

#include <libopencm3/stm32/rcc.h>

#include <stdlib.h>

#define ONEWIRE_USART3
#define MAXDEVICES_ON_THE_BUS 3

#include "OneWire.h"
#include "ks0108.h"

extern int board_setup(void);
void exp01();
void exp02();

int main(void) {

    board_setup();
    ks0108_init();

    uint32_t dd = 8000000;
    while (1) {
        ks0108_repaint(0);  //проверяем простую запись
        uint32_t delay = dd;
        while(delay--)
            __asm__("nop");
        exp01(); //проверяем чтение и перезапись прочитанного запись
        delay = dd;
        while(delay--)
                __asm__("nop");
        ks0108_paint(0);  //пишем простое число, например, 0 для стирания всего на экране
        delay = dd;
        while(delay--)
                __asm__("nop");
        exp02(); // спираль из точек
        delay = dd;
        while(delay--)
                __asm__("nop");
    }
    /* В любых нормальных обстоятельствах мы никогда не попадём сюда */
    return 0;
}

void exp01() {
    uint8_t chip, page, address;
    u_PortStruct_t cmd;
    //set cs
    for (chip = 1; chip < 3; chip++) {
        cmd.p.cs = chip;
        for (page = 0; page < 8; page++) {
            cmd.p.db = (uint8_t) (page | 0xb8);
            cmd.p.a0 = 0;
            cmd.p.rw = 0;
            ks0108_send(cmd);
            cmd.p.db = 0x40;
            cmd.p.a0 = 0;
            cmd.p.rw = 0;
            ks0108_send(cmd);
            uint8_t buffer[64];
            ks0108_receive(chip); // ОБЯЗАТЕЛЬНОЕ ФИКТИВНОЕ ЧТЕНИЕ!
            for (address = 0; address < 64; address++) {
                buffer[address] = ks0108_receive(chip);
            }
            cmd.p.db = (uint8_t) (page | 0xb8);
            cmd.p.a0 = 0;
            cmd.p.rw = 0;
            ks0108_send(cmd);
            cmd.p.db = 0x40;
            cmd.p.a0 = 0;
            cmd.p.rw = 0;
            ks0108_send(cmd);
            for (address = 0; address < 64; address++) {
                cmd.p.a0 = 1;
                cmd.p.rw = 0;
                uint8_t p = buffer[address];
                p = (p == 0x55) ? 0xaa : 0x55;
                cmd.p.db = p;
                ks0108_send(cmd);
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

