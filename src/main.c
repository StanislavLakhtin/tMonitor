//
// Created by Stanislav Lakhtin on 19/11/2016.
//

#define ONEWIRE_USART3
#define MAXDEVICES_ON_THE_BUS 3

#include <OneWire.h>
#include "OneWire.h"
#include "ks0108.h"

static void clock_setup(void)
{
  rcc_clock_setup_in_hse_8mhz_out_72mhz();

  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_GPIOC);

  rcc_periph_clock_enable(RCC_AFIO);

  rcc_periph_clock_enable(RCC_USART3);
}

static void gpio_setup(void) {
  nvic_enable_irq(NVIC_USART3_IRQ);

  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_10_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, GPIO_ALL);

  gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_10_MHZ,
                GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART3_TX);

  gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_10_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, GPIO0); //LCD(KS0108) RESET PIN

  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_10_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

  gpio_set(GPIOB, GPIO0);

  AFIO_MAPR |= AFIO_MAPR_SWJ_CFG_FULL_SWJ_NO_JNTRST;
}

void usart3_isr(void) {
  /* Проверяем, что мы вызвали прерывание из-за RXNE. */
  if (((USART_CR1(USART3) & USART_CR1_RXNEIE) != 0) &&
      ((USART_SR(USART3) & USART_SR_RXNE) != 0)) {

    /* Получаем данные из периферии и сбрасываем флаг*/
    rc_buffer[2] = usart_recv_blocking(USART3);
    recvFlag &= ~(1 << 2);
  }
}

void shortDelay(uint32_t);
void test01(uint8_t color); //линии из точек
void test02(uint8_t color); //рисование точками спирали
void test03(); //вывод текста
void test04(uint8_t color); //рисование окружности
void test05(); //управление Start Line на каждом из чипов
void test06(); //позиционирование текста по любому адресу
void test07();
void testDS18B20(); //тест с использованием библиотеки для DS18x20

OneWire ow;

int main(void) {

  clock_setup();
  gpio_setup();

  ks0108_init();

  while (1) {
    ks0108_paint(0);
    test02(BLACK); // спираль из точек
    shortDelay(8000000);
    test02(WHITE); // спираль из точек
    shortDelay(8000000);
    /*test01(BLACK);
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
    test07();
    shortDelay(8000000);*/
    ks0108_paint(0);
    testDS18B20();
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
  typedef struct {
    wchar_t *c;
    uint8_t x;
    uint8_t y;
    uint8_t dir;
  } charStr_t;
  charStr_t privet[] = {
      {L"П", 5,0,1},
      {L"р", 13,7,0},
      {L"и", 23,3,1},
      {L"в", 38,10,2},
      {L"е", 44,3,3},
      {L"т", 56,6,0},
      {L",", 61,5,1},
      {L"М", 65,5,3},
      {L"И", 75,4,0},
      {L"Р", 83,3,1},
      {L"!", 91,3,3},
  };
  ks0108_paint(0x00);
  uint8_t step = 2;
  uint8_t conc = 1;
  do {
    uint8_t i =0;
    for (;i<11;i++){
      ks0108_drawText(privet[i].x, privet[i].y, WHITE, privet[i].c);
      switch (privet[i].dir){
        default: {
          privet[i].y += step;
          break;
        }
        case 1: {
          privet[i].x -= step;
          break;
        }
        case 2: {
          privet[i].y += step;
          break;
        }
        case 3: {
          privet[i].x += step;
          break;
        }
      }
      ks0108_drawText(privet[i].x, privet[i].y, BLACK, privet[i].c);
      privet[i].dir += (privet[i].dir==3?-3:1);
      if (privet[i].y>63)
        conc = 0;
    }
    shortDelay(1000000);
  } while (conc);
}

void testDS18B20() {
  ow.usart = USART3;
  wchar_t *sT = L"Количество: ";
  wchar_t buffer[30];
  uint8_t offsetX = 4;
  uint8_t lHeight = 8;
  if (owResetCmd(&ow) != ONEWIRE_NOBODY) {
    ks0108_drawText(offsetX, 0, BLACK, L"Найдены сенсоры на шине");
    ks0108_drawText(offsetX, lHeight, BLACK, sT);
    uint8_t cnt = owScanCmd(&ow);
    ks0108_drawInt(offsetX+ks0108_textLength(sT), lHeight, BLACK, cnt, L"%d");
    int i = 0;
    for (; i<cnt; i++) {
      wchar_t t[30];
      Temperature temp;
      RomCode *r = &ow.ids[i];
      switch (ow.ids[i].family) {
        case DS18B20:
          temp = readTemperature(&ow, &ow.ids[i], true);
          swprintf(buffer, 40, L"(DS18B20:%02x %02x %02x %02x %02x %02x) температура %+000d.%d",
           r->code[0], r->code[1], r->code[2],
           r->code[3], r->code[4], r->code[5], temp.inCelsus, temp.frac);
          break;
        case DS18S20:
          temp = readTemperature(&ow, &ow.ids[i], true);
          swprintf(buffer, 40, L"(DS18S20:%02x %02x %02x %02x %02x %02x) температура %+000d.%d",
                   r->code[0], r->code[1], r->code[2],
                   r->code[3], r->code[4], r->code[5], temp.inCelsus, temp.frac);
          break;
        default:
          swprintf(buffer, 40, L"(Unknown:%02x %02x %02x %02x %02x %02x)",
                   r->code[0], r->code[1], r->code[2],
                   r->code[3], r->code[4], r->code[5]);
          break;
      }
      ks0108_drawText(offsetX, lHeight*(2+i), BLACK, buffer);
    }
  } else {
    ks0108_drawText(10,10,BLACK,L"Никого нет");
  }
}

void test07() {
  ks0108_paint(0);
  int16_t i=0, ii;
  wchar_t buffer[30];
  uint8_t ys[] = {24, 32, 40};
  wchar_t *justString = L"Форматирование:";
  wchar_t *format[] = {L"%+00d", L"%d", L"%#04X"};
  uint8_t offset = 12;
  for (;i<3;i++){
    ks0108_drawText(offset, ys[i], BLACK, justString);
  }
  uint16_t jsLength = offset+ks0108_textLength(justString);
  for (ii=254;ii>-100;ii--){
    for (i = 0;i<3;i++){
      swprintf(buffer, 30, format[i], ii);
      ks0108_drawText(jsLength,ys[i], BLACK, buffer);
    }
    shortDelay(400000);
    for (i = 0;i<3;i++){
      swprintf(buffer, 30, format[i], ii);
      ks0108_drawText(jsLength,ys[i], WHITE, buffer);
    }
  }
}

