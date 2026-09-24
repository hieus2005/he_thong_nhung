#ifndef MAX7219_H
#define MAX7219_H

#include "stm32f10x.h"

// Các thanh ghi cấu hình của MAX7219
#define MAX7219_REG_DECODE_MODE   0x09
#define MAX7219_REG_INTENSITY     0x0A
#define MAX7219_REG_SCAN_LIMIT    0x0B
#define MAX7219_REG_SHUTDOWN      0x0C
#define MAX7219_REG_DISPLAY_TEST  0x0F

void MAX7219_Init(void);
void MAX7219_Write(uint8_t address, uint8_t data);
void MAX7219_DisplayPattern(uint8_t *pattern);

#endif

