#include "stm32f10x.h"

volatile unsigned int cnt_01hz = 0;
volatile unsigned int cnt_1hz  = 0;
volatile unsigned int cnt_10hz = 0;

void SysTick_Handler(void) {
    cnt_01hz++;
    cnt_1hz++;
    cnt_10hz++;

    if (cnt_01hz >= 5000) {         // 0.1Hz -> toggle mỗi 5000ms
        GPIOA->ODR ^= (1 << 0);     // LED1 - PA0
        cnt_01hz = 0;
    }
    if (cnt_1hz >= 500) {           // 1Hz -> toggle mỗi 500ms
        GPIOA->ODR ^= (1 << 1);     // LED2 - PA1
        cnt_1hz = 0;
    }
    if (cnt_10hz >= 50) {           // 10Hz -> toggle mỗi 50ms
        GPIOA->ODR ^= (1 << 2);     // LED3 - PA2
        cnt_10hz = 0;
    }
}

int main(void) {
    RCC->APB2ENR |= (1 << 2);       // bật clock GPIOA
    GPIOA->CRL = 0x33333333;        // PA0-PA7 = output push-pull

    STK_LOAD = 999;                 // 1MHz / 1000 = tick mỗi 1ms
    STK_VAL  = 0;
    STK_CTRL = (1 << 0) | (1 << 1); // bật SysTick + bật ngắt (TICKINT)

    __asm volatile ("cpsie i");     // bật ngắt toàn cục (global interrupt enable)

    while (1) {
        // mọi xử lý nằm trong SysTick_Handler, vòng lặp chính để trống
    }
}