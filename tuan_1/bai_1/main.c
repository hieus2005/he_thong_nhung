#include "stm32f10x.h"

void delay_ms(uint16_t t);

int main()
{
	RCC->APB2ENR |= 0xFC;	/* Bat xung nhip clock cho cac cong GPIO A den F */

	GPIOC->CRH = 0x44344444;	/* Cau hinh chan PC13 lam ngo ra (output push-pull) */

	while(1)
	{
		GPIOC->ODR ^= (1<<13);	/* Dao trang thai logic cua chan PC13 (bat/tat LED) */
		delay_ms(1000);         /* Tre khoang 1000 mili-giay (1 giay) */
	}
}