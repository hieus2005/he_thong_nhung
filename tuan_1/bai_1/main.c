#include <stdint.h>

/* RCC */
#define RCC_APB2ENR (*(volatile uint32_t *)0x40021018)

/* GPIOC */
#define GPIOC_CRH  (*(volatile uint32_t *)0x40011004)
#define GPIOC_ODR  (*(volatile uint32_t *)0x4001100C)

void delay(void)
{
    for(volatile uint32_t i = 0; i < 3000000; i++);
}

int main(void)
{
    /* Bật clock GPIOC (IOPCEN - bit 4) */
    RCC_APB2ENR |= (1 << 4);

    /* PC13: Output Push-Pull 2MHz
       MODE13 = 10
       CNF13  = 00
       => 0010 */
    GPIOC_CRH &= ~(0xF << 20);
    GPIOC_CRH |=  (0x2 << 20);

    /* Ban đầu tắt LED */
    GPIOC_ODR |= (1 << 13);

    while(1)
    {
        /* LED sáng */
        GPIOC_ODR &= ~(1 << 13);
        delay();

        /* LED tắt */
        GPIOC_ODR |= (1 << 13);
        delay();
    }
}
