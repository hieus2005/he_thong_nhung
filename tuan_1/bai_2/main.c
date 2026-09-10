#include "stm32f10x.h"  /* Thu vien dinh nghia thanh ghi dong STM32F10x */
#include <stdint.h>     /* Thu vien chuan ho tro cac kieu du lieu */

void delay_ms(uint16_t t); /* Khai bao nguyen mau ham tao tre thoi gian */

int main() {
    RCC->APB2ENR |= 0xFC;      /* Bat xung nhip clock cho cac cong GPIO tu A den F */
    GPIOA->CRL = 0x33333333;   /* Cau hinh cac chan tu PA0 den PA7 lam ngo ra (Output Push-Pull) */

    while(1) {
        int8_t n;

        GPIOA->ODR = 0x00;     /* Tat toan bo cac LED truoc khi chay hieu ung */

        for(n = 0; n <= 7; n++)   /* Vong lap chay từ trai sang phai (PA0 den PA7) */
        {
            GPIOA->ODR = (1<<n);  /* Bat sang LED tai chan PAn, cac chan kha tat */
            delay_ms(500);        /* Tre 500ms */
        }

        for(n = 7; n >= 0; n--)   /* Vong lap chay nguoc tu phai ve trai (PA7 ve PA0) */
        {
            GPIOA->ODR = (1<<n);  /* Bat sang LED tai chan PAn, cac chan khac tat */
            delay_ms(500);        /* Tre 500ms */
        }
    }
}

void delay_ms(uint16_t t) {
    for (uint32_t i = 0; i < t * 800; i++) { /* Vong lap tao do tre tinh theo mili-giay */
        __asm("nop");         /* Lenh trong cua Assembly de tieu ton chu ky may */
    }
}