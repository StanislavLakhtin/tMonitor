//-----------------------------------------------------------------------------
// Данная функция осуществляет сканирование сети 1-wire и записывает найденные
//   ID устройств в массив buf, по 8 байт на каждое устройство.
// переменная num ограничивает количество находимых устройств, чтобы не переполнить
// буфер.
//-----------------------------------------------------------------------------
uint8_t OW_Scan(uint8_t *buf, uint8_t num) {

        uint8_t found = 0;
        uint8_t *lastDevice;
        uint8_t *curDevice = buf;
        uint8_t numBit, lastCollision, currentCollision, currentSelection;

        lastCollision = 0;
        while (found < num) {
                numBit = 1;
                currentCollision = 0;

                // посылаем команду на поиск устройств
                OW_Send(OW_SEND_RESET, (uint8_t*)"\xf0", 1, 0, 0, OW_NO_READ);

                for (numBit = 1; numBit <= 64; numBit++) {
                        // читаем два бита. Основной и комплементарный
                        OW_toBits(OW_READ_SLOT, ow_buf);
                        OW_SendBits(2);

                        if (ow_buf[0] == OW_R_1) {
                                if (ow_buf[1] == OW_R_1) {
                                        // две единицы, где-то провтыкали и заканчиваем поиск
                                        return found;
                                } else {
                                        // 10 - на данном этапе только 1
                                        currentSelection = 1;
                                }
                        } else {
                                if (ow_buf[1] == OW_R_1) {
                                        // 01 - на данном этапе только 0
                                        currentSelection = 0;
                                } else {
                                        // 00 - коллизия
                                        if (numBit < lastCollision) {
                                                // идем по дереву, не дошли до развилки
                                                if (lastDevice[(numBit - 1) >> 3]
                                                                & 1 << ((numBit - 1) & 0x07)) {
                                                        // (numBit-1)>>3 - номер байта
                                                        // (numBit-1)&0x07 - номер бита в байте
                                                        currentSelection = 1;

                                                        // если пошли по правой ветке, запоминаем номер бита
                                                        if (currentCollision < numBit) {
                                                                currentCollision = numBit;
                                                        }
                                                } else {
                                                        currentSelection = 0;
                                                }
                                        } else {
                                                if (numBit == lastCollision) {
                                                        currentSelection = 0;
                                                } else {
                                                        // идем по правой ветке
                                                        currentSelection = 1;

                                                        // если пошли по правой ветке, запоминаем номер бита
                                                        if (currentCollision < numBit) {
                                                                currentCollision = numBit;
                                                        }
                                                }
                                        }
                                }
                        }

                        if (currentSelection == 1) {
                                curDevice[(numBit - 1) >> 3] |= 1 << ((numBit - 1) & 0x07);
                                OW_toBits(0x01, ow_buf);
                        } else {
                                curDevice[(numBit - 1) >> 3] &= ~(1 << ((numBit - 1) & 0x07));
                                OW_toBits(0x00, ow_buf);
                        }
                        OW_SendBits(1);
                }
                found++;
                lastDevice = curDevice;
                curDevice += 8;
                if (currentCollision == 0)
                        return found;

                lastCollision = currentCollision;
        }

        return found;
}