#include "stm32f10x.h"
#include <stdint.h>
void delay_ms(uint16_t t);

int main(void) {
    /* Bat clock cho PORTA (Nut nhan o PA0) va PORTC (LED o PC13) */
    RCC->APB2ENR |= (1 << 2) | (1 << 4);  

    /* PA0: Input Pull-up (Nut nhan noi len 3.3V, nhan xuong 0V) */
    GPIOA->CRL = (GPIOA->CRL & 0xFFFFFFF0) | 0x00000008;
    GPIOA->ODR |= (1 << 0);               /* Keo len cho PA0 */

    /* PC13: Output Push-Pull 2MHz  */
    GPIOC->CRH = (GPIOC->CRH & 0xFF0FFFFF) | 0x00200000;

    uint8_t last_btn_state = 1;           /* Trang thai truoc cua nut (mac dinh 1 do keo len) */

    while(1) {
        /* Doc trang thai nut nhan tai PA0 */
        uint8_t current_btn_state = (GPIOA->IDR & (1 << 0)) ? 1 : 0;

        if (last_btn_state == 0 && current_btn_state == 1) {
            /* Dao trang thai LED tai PC13 */
            GPIOC->ODR ^= (1 << 13);
        }

        last_btn_state = current_btn_state; /* Luu lai trang thai hien tai cho chu ky sau */
        delay_ms(20);                       /* Do tre chong doi phim  */
    }
}

void delay_ms(uint16_t t) {
    for (uint32_t i = 0; i < t * 800; i++) {
        __asm("nop");
    }
}