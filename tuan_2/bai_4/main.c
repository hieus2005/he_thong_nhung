#include "stm32f10x.h"

int main(void) {
    RCC->APB1ENR |= (1 << 0);   // bật clock TIM2
    RCC->APB2ENR |= (1 << 2);   // bật clock GPIOA

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin   = (1<<0) | (1<<1) | (1<<2) | (1<<3);   // PA0-PA3
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    TIM_TimeBaseInitTypeDef timebase;
    timebase.TIM_Prescaler = 7;      // 8MHz / 8 = 1MHz
    timebase.TIM_Period    = 999;    // 1MHz / 1000 = 1kHz
    TIM_TimeBaseInit(TIM2, &timebase);

    TIM_OCInitTypeDef oc;

    oc.TIM_Pulse = 100;  TIM_OC1Init(TIM2, &oc);   // duty 10%
    oc.TIM_Pulse = 300;  TIM_OC2Init(TIM2, &oc);   // duty 30%
    oc.TIM_Pulse = 500;  TIM_OC3Init(TIM2, &oc);   // duty 50%
    oc.TIM_Pulse = 700;  TIM_OC4Init(TIM2, &oc);   // duty 70%

    TIM_Cmd(TIM2, 1);   // bật timer, PWM chạy nền

    while (1) {
        // không cần làm gì thêm, PWM tự chạy
    }
}