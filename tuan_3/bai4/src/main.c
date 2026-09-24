#include "stm32f10x.h"
#include <stdint.h>

// Hàm SystemInit để startup.s gọi khi chip khởi động
void SystemInit(void) {
    // Sử dụng dao động nội mặc định của chip
}

#define SAMPLE_COUNT 100 // 100 mẫu trong 1 giây (100Hz)

volatile uint16_t adc_buffer[SAMPLE_COUNT];
volatile uint8_t flag_half_complete = 0;
volatile uint8_t flag_full_complete = 0;

// Gửi 1 ký tự qua USART1
void UART1_SendChar(char c) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, c);
}

// Gửi chuỗi qua USART1
void UART1_SendString(const char *str) {
    while (*str) {
        UART1_SendChar(*str++);
    }
}

// Chuyển số nguyên không dấu thành chuỗi và gửi kèm \n\r theo yêu cầu
void UART1_SendNumber(uint16_t num) {
    char buf[8];
    int i = 0;
    if (num == 0) {
        UART1_SendChar('0');
    } else {
        while (num > 0) {
            buf[i++] = (num % 10) + '0';
            num /= 10;
        }
        while (i > 0) {
            UART1_SendChar(buf[--i]);
        }
    }
    UART1_SendString("\n\r");
}

// Gửi mảng mẫu dữ liệu
void send_adc_data(volatile uint16_t *buf, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        UART1_SendNumber(buf[i]);
    }
}

// Cấu hình Clock và GPIO
void GPIO_Configuration(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1 | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // PA0: ADC Channel 0 (Nối chân giữa biến trở)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PA9: USART1_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PA10: USART1_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

// Cấu hình USART1 baudrate 115200
void USART1_Configuration(void) {
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

// Cấu hình Timer 3 tạo tần số 100Hz kích hoạt ADC
void TIM3_Configuration(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // Với tần số mặc định 8MHz: 8MHz / (79 + 1) = 100kHz. 100kHz / (999 + 1) = 100Hz
    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_Prescaler = 79;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // Chọn TRGO kích hoạt khi có sự kiện Update của Timer
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
    TIM_Cmd(TIM3, ENABLE);
}

// Cấu hình DMA1 Channel 1 và ngắt Half/Full Transfer
void DMA1_Configuration(void) {
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)adc_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = SAMPLE_COUNT;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    // Bật cả ngắt Half-Transfer (HT) và Transfer-Complete (TC)
    DMA_ITConfig(DMA1_Channel1, DMA_IT_HT | DMA_IT_TC, ENABLE);

    // Cấu hình NVIC cho ngắt DMA1 Channel 1
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);
}

// Cấu hình ADC1 kích hoạt bởi Timer 3 TRGO
void ADC1_Configuration(void) {
    ADC_InitTypeDef ADC_InitStructure;

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);

    ADC_DMACmd(ADC1, ENABLE);
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);

    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

// Hàm ngắt DMA1 Channel 1
void DMA1_Channel1_IRQHandler(void) {
    if (DMA_GetITStatus(DMA1_IT_HT1) != RESET) {
        DMA_ClearITPendingBit(DMA1_IT_HT1);
        flag_half_complete = 1;
    }
    if (DMA_GetITStatus(DMA1_IT_TC1) != RESET) {
        DMA_ClearITPendingBit(DMA1_IT_TC1);
        flag_full_complete = 1;
    }
}

int main(void) {
    GPIO_Configuration();
    USART1_Configuration();
    DMA1_Configuration();
    ADC1_Configuration();
    TIM3_Configuration();

    UART1_SendString("STM32 Ready! Reading ADC @ 100Hz...\n\r");

    while (1) {
        if (flag_half_complete) {
            flag_half_complete = 0;
            send_adc_data(&adc_buffer[0], SAMPLE_COUNT / 2);
        }

        if (flag_full_complete) {
            flag_full_complete = 0;
            send_adc_data(&adc_buffer[SAMPLE_COUNT / 2], SAMPLE_COUNT / 2);
        }
    }
}
