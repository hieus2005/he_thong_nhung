#include "stm32f10x.h"

void delay_ms(uint16_t t);

int main()
{
	RCC->APB2ENR |= 0xFC;	/* Bật xung nhịp clock cho các cổng GPIO A đến F */

	GPIOC->CRH = 0x44344444;	/* Cấu hình chân PC13 làm ngõ ra (output push-pull) */

	while(1)
	{
		GPIOC->ODR ^= (1<<13);	/* Đảo trạng thái logic của chân PC13 (bật/tắt LED) */
		delay_ms(1000);         /* Trễ khoảng 1000 mili-giây (1 giây) */
	}
}