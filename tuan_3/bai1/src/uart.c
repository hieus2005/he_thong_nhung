#include "uart.h"

void USART1_Init(uint32_t baud_rate)
{
    // Bật Clock cho USART1 (Bit 14) và GPIOA (Bit 2)
    RCC->APB2ENR |= (1 << 14) | (1 << 2);

    // Cấu hình PA9 là TX (Alternate function push-pull, 50MHz)
    GPIOA->CRH &= ~(unsigned int)(0xF << 4);
    GPIOA->CRH |= (0xB << 4);

    // Cấu hình PA10 là RX (Input floating)
    GPIOA->CRH &= ~(unsigned int)(0xF << 8);
    GPIOA->CRH |= (0x4 << 8);

    // Cấu hình Baudrate dựa trên clock PCLK2 = 72MHz
    USART1->BRR = (unsigned short)(72000000 / baud_rate);

    // Bật USART1, TX, RX, ngắt RXNE (Bit 2, 3, 13, 5 trong CR1)
    USART1->CR1 |= (1 << 2) | (1 << 3) | (1 << 13) | (1 << 5);

    // BẬT CHẾ ĐỘ DMA TX: DMAT (Bit 7 trong thanh ghi CR3)
    USART1->CR3 |= (1 << 7);
}

void USART1_Send_Char(char chr)
{
    while (!(USART1->SR & (1 << 7))); // Chờ TXE trống
    USART1->DR = chr;
}

void USART1_Send_String(char *str)
{
    while (*str) {
        while (!(USART1->SR & (1 << 7)));
        USART1->DR = *str++;
    }
}

void USART1_Send_Data(uint8_t *data, uint8_t length)
{
    for (int i = 0; i < length; i++) {
        while (!(USART1->SR & (1 << 7)));
        USART1->DR = data[i];
    }
}

void USART1_Send_Number(int16_t num)
{
    if (num < 0) {
        USART1_Send_Char('-');
        num = -num;
    }
    uint8_t length = 0;
    uint8_t temp[10];
    if (num == 0) {
        USART1_Send_Char('0');
        return;
    } else {
        while (num != 0) {
            uint8_t value = num % 10;
            temp[length++] = value + '0';
            num /= 10;
        }
        for (int i = length - 1; i >= 0; i--) {
            USART1_Send_Char(temp[i]);
        }
    }
}

void USART1_Send_Float(float num)
{
    if (num < 0) { USART1_Send_Char('-'); num = -num; }

    int16_t integer = (int16_t)num;
    int16_t frac = (int16_t)((num - (float)integer) * 1000.0f + 0.5f);

    if (frac >= 1000) { integer++; frac -= 1000; }

    USART1_Send_Number(integer);
    USART1_Send_Char('.');

    if (frac < 100) USART1_Send_Char('0');
    if (frac < 10)  USART1_Send_Char('0');
    USART1_Send_Number(frac);
}

void USART1_Send_Hex(uint8_t num)
{
    uint8_t temp;
    temp = num >> 4;
    if (temp > 9) {
        temp += 0x37;
    } else {
        temp += 0x30;
    }
    USART1_Send_Char(temp);
    temp = num & 0x0F;
    if (temp > 9) {
        temp += 0x37;
    } else {
        temp += 0x30;
    }
    USART1_Send_Char(temp);
}
