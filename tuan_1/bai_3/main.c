#include "stm32f10x.h"
#include <stdint.h>
void delay_ms(uint16_t t);

int main(void) {
    /* Bat clock cho PORTA (Nut nhan) va PORTB (LED) */
    RCC->APB2ENR |= (1 << 2) | (1 << 3);  

    /* PA0 - PA7: Input Pull-up */
    GPIOA->CRL = 0x88888888;              /* Cau hinh cac chan PA0-PA7 o che do Input co dien tro keo len/xuong */
    GPIOA->ODR |= 0x00FF;                 /* Thiet lap trang thai keo len (Pull-up) cho PA0-PA7 */
 
	/*Vi PA8 den PA15 khong the dung 2 PA13 va PA14 nen em xin phep su dung PB8 den PB15 */
    /* PB8 - PB15: Output Push-Pull 10MHz */
    GPIOB->CRH = 0x11111111;              /* Cau hinh cac chan PB8-PB15 lam ngo ra kieu Push-Pull, toc do 10MHz */

    while(1) {
        /* Doc PA0-PA7 */
        uint8_t input_val = (uint8_t)(GPIOA->IDR & 0x00FF); /* Doc gia tri muc logic tu 8 chan dau vao cua PORTA */
        /* Dao du lieu */
        uint8_t inv_val = ~input_val;     /* Dao nguoc trang thai cac bit (0 thanh 1, 1 thanh 0) */
        /* Xuat ra PB8-PB15 */
        GPIOB->ODR = ((uint16_t)inv_val) << 8; /* Dich gia tri da dao sang 8 bit cao va xuat ra PORTB (tu PB8 den PB15) */

        delay_ms(10);                     /* Tao do tre 10 mili-giay de chong doi phim va giam tan suat quet */
    }
}

void delay_ms(uint16_t t) {
    for (uint32_t i = 0; i < t * 800; i++) { /* Vong lap tao do tre bang lenh trong */
        __asm("nop");                     /* Lenh Assembly tieu ton chu ky may de giu nhip tre */
    }
}