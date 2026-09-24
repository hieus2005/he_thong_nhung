#include "stm32f10x.h"
#include "uart.h"
#include "dma.h"
#include "mpu6050.h"
#include <stdio.h>

#define TX_BUFFER_SIZE 128
char tx_buffer[TX_BUFFER_SIZE];
MPU6050_Data_t imu_data;

// Hàm rỗng ngăn GCC gọi SystemInit mặc định
void SystemInit(void) {
}

// Cấu hình Clock hệ thống 72MHz
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

void Delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 4000; i++) {
        __NOP();
    }
}

int main(void) {
    SystemClock_Config();

    // Khởi tạo UART1 + DMA1 Channel 4
    USART1_Init(115200);
    DMA1_USART1_TX_Init();

    // Khởi tạo MPU6050
    if (MPU6050_Init() != 0) {
        int len = sprintf(tx_buffer, "[ERROR] MPU6050 Init Failed!\r\n");
        DMA1_USART1_TX_Send(tx_buffer, len);
        while (1);
    }

    int len = sprintf(tx_buffer, "[OK] MPU6050 Init Success!\r\n");
    DMA1_USART1_TX_Send(tx_buffer, len);
    Delay_ms(500);

    while (1) {
        // Đọc dữ liệu IMU qua I2C
        MPU6050_Read_All(&imu_data);

        // Chờ DMA đợt trước phát xong
        while ((DMA1_Channel4->CCR & (1 << 0)) && !(DMA1->ISR & (1 << 13)));

        // Đóng gói chuỗi dữ liệu
        len = sprintf(tx_buffer, ":IMU:%d,%d,%d,%d,%d,%d:\r\n",
                      imu_data.Accel_X, imu_data.Accel_Y, imu_data.Accel_Z,
                      imu_data.Gyro_X, imu_data.Gyro_Y, imu_data.Gyro_Z);

        // Bắn dữ liệu lên UART bằng DMA
        DMA1_USART1_TX_Send(tx_buffer, len);

        // Đổi từ 100ms lên 500ms (truyền 2 lần/giây)
        Delay_ms(500);
    }
}
