#include "stm32f10x.h"
#include "uart.h"

#define MAX_LEN 100
char rx_buffer[MAX_LEN];
volatile uint8_t rx_index = 0;
volatile uint8_t msg_complete = 0;

// Hàm rỗng dể ng?n GCC g?i SystemInit t? system_stm32f10x.c
void SystemInit(void) {
}

// Hàm c?u hình Clock h? th?ng ch?y ? 72MHz s? d?ng th?ch anh ngoài c?a b?n
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

// Trình ph?c v? ng?t USART1
void USART1_IRQHandler(void) {
    if (USART1->SR & (1 << 5)) {
        char data = (char)USART1->DR;
        
        if (data == '!') {
            rx_buffer[rx_index] = '\0';
            msg_complete = 1;
        } else {
            if (rx_index < MAX_LEN - 1) {
                rx_buffer[rx_index++] = data;
            }
        }
    }
}

int main(void) {
    // C?u hình th?ch anh ngoài lên 72MHz
    SystemClock_Config();

    // Kh?i t?o UART v?i Baudrate 115200 theo dúng thi?t k? c?a b?n
    USART1_Init(115200);
    
    // B?t ng?t USART1 trên b? di?u khi?n trung tâm NVIC
    NVIC_EnableIRQ(USART1_IRQn);

    char prefix[] = " 20261-03_Nhom01:  ";

    while(1) {
        if (msg_complete) {
            USART1_Send_String(prefix);
            USART1_Send_String(rx_buffer);
            USART1_Send_String("\r\n");
            
            rx_index = 0;
            msg_complete = 0;
        }
    }
}
