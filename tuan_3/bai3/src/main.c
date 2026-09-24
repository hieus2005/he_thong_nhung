#include "stm32f10x.h"
#include "uart.h"     //[cite: 3]
#include "dma.h"
#include <stdio.h>
#include <string.h>

#define TX_BUFFER_SIZE 64
char tx_buffer[TX_BUFFER_SIZE];
uint32_t btn_count = 0;

// Hàm rỗng để ngăn GCC gọi SystemInit mặc định[cite: 3]
void SystemInit(void) {
}

// Cấu hình Clock hệ thống chạy 72MHz chuẩn như bài mẫu[cite: 3]
void SystemClock_Config(void) {
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    FLASH->ACR |= FLASH_ACR_PRFTBE; 
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2; 

    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; 
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; 

    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
    RCC->CFGR |= (RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

// Cấu hình Nút Nhấn tại chân PA0 (Chế độ Input Pull-Up)
void GPIO_Button_Init(void) {
    RCC->APB2ENR |= (1 << 2); // Bật Clock GPIOA

    // Cấu hình PA0: Input Pull-up / Pull-down (CNF=10, MODE=00)
    GPIOA->CRL &= ~(0xF << 0);
    GPIOA->CRL |= (0x8 << 0);
    GPIOA->ODR |= (1 << 0);   // Bật Kéo lên (Pull-Up) -> Nhả nút = 1, Nhấn nút = 0
}

// [ĐÃ SỬA] Thêm `volatile` để GCC -O2 không tối ưu xóa bỏ vòng lặp Delay
void Delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 4000; i++) {
        __NOP();
    }
}

int main(void) {
    // 1. Cấu hình Clock 72MHz[cite: 3]
    SystemClock_Config();

    // 2. Khởi tạo UART1 Baudrate 115200 và DMA1 Channel 4[cite: 3]
    USART1_Init(115200);
    DMA1_USART1_TX_Init();

    // 3. Khởi tạo Nút nhấn PA0
    GPIO_Button_Init();

    while (1) {
        // Kiểm tra xem nút bấm PA0 có được nhấn hay không (Mức 0)
        if (!(GPIOA->IDR & (1 << 0))) {
            Delay_ms(20); // Chống dội lúc NHẤN NÚT (Debounce Press)
            
            if (!(GPIOA->IDR & (1 << 0))) {
                // Đảm bảo lượt truyền DMA trước đó đã xong
                while ((DMA1_Channel4->CCR & (1 << 0)) && !(DMA1->ISR & (1 << 13)));

                // Tăng biến đếm nút nhấn
                btn_count++;

                // Đóng gói bản tin theo định dạng :BTN:<count>:\n\r
                int len = sprintf(tx_buffer, ":BTN:%lu:\n\r", btn_count);

                // Gọi lệnh phát DMA
                DMA1_USART1_TX_Send(tx_buffer, len);

                // Chờ cho người dùng thả nút nhấn ra hẳn
                while (!(GPIOA->IDR & (1 << 0)));

                // [ĐÃ THÊM] Chống dội lúc THẢ NÚT (Debounce Release)
                Delay_ms(20); 
            }
        }
    }
}
