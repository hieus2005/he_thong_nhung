#include <stdint.h>

/* RCC */
#define RCC_APB2ENR (*(volatile uint32_t *)0x40021018)

/* GPIOA */
#define GPIOA_CRL (*(volatile uint32_t *)0x40010800)
#define GPIOA_ODR (*(volatile uint32_t *)0x4001080C)

/* Hàm tạo trễ */
void delay(void)
{
    for (volatile uint32_t i = 0; i < 300000; i++);
}

int main(void)
{
    /* Bật clock GPIOA */
    RCC_APB2ENR |= (1 << 2);

    /* PA0-PA7: Output Push-Pull 2MHz */
    GPIOA_CRL = 0x22222222;

    uint8_t led = 0;
    int8_t huong = 1;

    while (1)
    {
        /* Chỉ bật một LED */
        GPIOA_ODR = (1 << led);

        delay();

        led += huong;

        if (led == 7)
            huong = -1;

        if (led == 0)
            huong = 1;
    }
}
