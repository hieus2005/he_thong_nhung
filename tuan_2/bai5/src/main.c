#include "stm32f10x.h"
#include "uart.h"
#include <string.h>
#include <stdlib.h>

#define MAX_LEN 50

char rx_buffer[MAX_LEN];
volatile uint8_t rx_index = 0;
volatile uint8_t msg_complete = 0;

// Biến lưu trạng thái của LED
uint8_t led_status = 0;       // 0: OFF, 1: ON
uint8_t current_pwm_pct = 50; // Mặc định 50%

void SystemInit(void) {}

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

// Cấu hình PWM trên chân PA1 (Timer 2 - Channel 2)
void PWM_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // Cấp xung clock cho GPIOA và TIM2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // Cấu hình PA1 là Alternate Function Push-Pull (để Timer điều khiển)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Cấu hình Timer 2: Tần số PWM = 1 kHz
    // Xung nhịp cấp cho TIM2 là 72MHz. 
    // Prescaler = 720-1 -> Tần số đếm = 100 kHz.
    // Period = 100-1 -> Timer đếm từ 0->99 rồi tràn. Tần số tràn = 1 kHz.
    TIM_TimeBaseStructure.TIM_Prescaler = 720 - 1;
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // Cấu hình Kênh 2 (Channel 2) của TIM2 ở chế độ PWM
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0; // Bắt đầu ở độ sáng 0% (Tắt)
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_Cmd(TIM2, ENABLE);
}

// Trình phục vụ ngắt UART
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

// Hàm cập nhật độ sáng thực tế cho LED
void Update_LED_Hardware(void) {
    if (led_status == 1) {
        TIM_SetCompare2(TIM2, current_pwm_pct); // Sáng theo cấu hình PWM
    } else {
        TIM_SetCompare2(TIM2, 0); // Tắt hẳn
    }
}

// Hàm xử lý chuỗi lệnh từ máy tính
void Process_Command(void) {
    if (strcmp(rx_buffer, "ON") == 0) {
        led_status = 1;
        Update_LED_Hardware();
        USART1_Send_String("-> LED is ON\r\n");
        
    } else if (strcmp(rx_buffer, "OFF") == 0) {
        led_status = 0;
        Update_LED_Hardware();
        USART1_Send_String("-> LED is OFF\r\n");
        
    } else if (strncmp(rx_buffer, "PWM:", 4) == 0) {
        // Trích xuất con số phần trăm (bỏ qua 4 ký tự "PWM:")
        int val = atoi(&rx_buffer[4]);
        
        if (val >= 0 && val <= 100) {
            current_pwm_pct = (uint8_t)val;
            Update_LED_Hardware(); // Hàm này sẽ tự kiểm tra trạng thái ON/OFF để set phần cứng
            
            USART1_Send_String("-> PWM Configured to ");
            USART1_Send_Number(val);
            USART1_Send_String("%\r\n");
        } else {
            USART1_Send_String("-> Error: PWM must be 0-100\r\n");
        }
        
    } else if (strcmp(rx_buffer, "Status") == 0) {
        USART1_Send_String("-> Status: ");
        if (led_status) USART1_Send_String("ON, PWM: ");
        else USART1_Send_String("OFF, PWM: ");
        USART1_Send_Number(current_pwm_pct);
        USART1_Send_String("%\r\n");
        
    } else {
        USART1_Send_String("-> Unknown Command\r\n");
    }
}

int main(void) {
    SystemClock_Config();
    USART1_Init(115200);
    PWM_Init();
    
    NVIC_EnableIRQ(USART1_IRQn);

    USART1_Send_String("\r\n--- PWM LED CONTROLLER READY ---\r\n");
    USART1_Send_String("Commands: ON!, OFF!, PWM:xx%!, Status!\r\n");

    while(1) {
        if (msg_complete) {
            Process_Command();
            
            // Xóa buffer sẵn sàng nhận lệnh mới
            rx_index = 0;
            msg_complete = 0;
        }
    }
}
