#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "uart.h"

// Hàm rỗng để chặn GCC gọi SystemInit mặc định
void SystemInit(void) {}

// Cấu hình thạch anh ngoài lên 72MHz (giữ nguyên của bạn)
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

// Hàm tạo trễ tương đối tính bằng mili-giây (ở xung nhịp 72MHz)
void Delay_ms(volatile uint32_t ms) {
    ms *= 8000; 
    while(ms--) { __NOP(); }
}

// Hàm cấu hình ADC1 trên chân PA0
void ADC1_Init(void) {
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. Cấp Clock cho ADC1 và GPIOA
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

    // 2. Cấu hình bộ chia tần số cho ADC (Clock ADC tối đa là 14MHz)
    // Clock APB2 = 72MHz -> Chia 6 = 12MHz (Hợp lệ)
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    // 3. Cấu hình chân PA0 ở chế độ Analog Input (Đầu vào tương tự)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 4. Thiết lập thông số cơ bản cho ADC1
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // 5. Bật ADC1
    ADC_Cmd(ADC1, ENABLE);

    // 6. Hiệu chuẩn (Calibration) để kết quả đo chính xác nhất
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

// Hàm lấy giá trị độ phân giải ADC (từ 0 đến 4095)
uint16_t ADC1_Read(uint8_t channel) {
    // Chọn kênh ADC cần đọc (Channel 0 ứng với PA0), thời gian lấy mẫu 55.5 chu kỳ
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_55Cycles5);
    
    // Ra lệnh bắt đầu chuyển đổi
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    
    // Chờ cờ EOC (End Of Conversion) bật lên báo hiệu đã đo xong
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    
    // Trả về kết quả (12-bit)
    return ADC_GetConversionValue(ADC1);
}

int main(void) {
    SystemClock_Config();
    
    // Khởi tạo UART với Baudrate 115200 (sử dụng thư viện uart.c của bạn)
    USART1_Init(115200);
    
    // Khởi tạo ADC1
    ADC1_Init();

    USART1_Send_String("\r\n--- STM32 ADC VOLTAGE METER ---\r\n");

    while(1) {
        // Bước 1: Đọc giá trị thô từ ADC (Kênh 0 - PA0)
        uint16_t adc_value = ADC1_Read(ADC_Channel_0);
        
        // Bước 2: Tính toán điện áp
        // - STM32F103 có ADC 12-bit -> Giá trị tối đa là 2^12 - 1 = 4095.
        // - Điện áp tham chiếu (VREF+) thường được nối cứng với 3.3V trên kit.
        // -> Công thức: Điện áp = (Giá trị ADC / 4095.0) * 3.3
        float voltage = ((float)adc_value / 4095.0f) * 3.3f;
        
        // Bước 3: Gửi dữ liệu lên máy tính
        USART1_Send_String("ADC Raw: ");
        USART1_Send_Number(adc_value);
        USART1_Send_String("  |  Dien ap: ");
        USART1_Send_Float(voltage);
        USART1_Send_String(" V\r\n");
        
        // Bước 4: Chờ khoảng 1 giây trước khi gửi tiếp
        Delay_ms(1000);
    }
}
