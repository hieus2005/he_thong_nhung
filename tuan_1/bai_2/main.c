#include "stm32f10x.h"  /* Thư viện định nghĩa thanh ghi dòng STM32F10x */
#include <stdint.h>     /* Thư viện chuẩn hỗ trợ các kiểu dữ liệu (uint16_t, int8_t...) */

void delay_ms(uint16_t t); /* Khai báo nguyên mẫu hàm tạo trễ thời gian */

int main() {
    RCC->APB2ENR |= 0xFC;      /* Bật xung nhịp clock cho các cổng GPIO từ A đến F */
    GPIOA->CRL = 0x33333333;   /* Cấu hình các chân từ PA0 đến PA7 làm ngõ ra (Output Push-Pull) */

    while(1) {
        int8_t n;

        GPIOA->ODR = 0x00;     /* Tắt toàn bộ các LED (đưa mức 0) trước khi chạy hiệu ứng */

        for(n = 0; n <= 7; n++)   /* Vòng lặp chạy từ trái sang phải (PA0 đến PA7) */
        {
            GPIOA->ODR = (1<<n);  /* Bật sáng LED tại chân PAn, các chân khác tắt */
            delay_ms(500);        /* Trễ 500ms */
        }

        for(n = 7; n >= 0; n--)   /* Vòng lặp chạy ngược từ phải về trái (PA7 về PA0) */
        {
            GPIOA->ODR = (1<<n);  /* Bật sáng LED tại chân PAn, các chân khác tắt */
            delay_ms(500);        /* Trễ 500ms */
        }
    }
}

void delay_ms(uint16_t t) {
    for (uint32_t i = 0; i < t * 800; i++) { /* Vòng lặp tạo độ trễ tính theo mili-giây */
        __asm("nop");         /* Lệnh trống của Assembly để tiêu tốn chu kỳ máy */
    }
}