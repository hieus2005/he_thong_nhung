#include "i2c.h"
#include "stm32f10x.h"

void I2C1_Init(void) {
    // 1. Bật Clock GPIOB và I2C1
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // 2. Cấu hình PB6 (SCL) và PB7 (SDA): Alternate Function Open-Drain (CNF=11, MODE=11 -> 0xF)
    GPIOB->CRL &= ~((0xF << 24) | (0xF << 28));
    GPIOB->CRL |=  ((0xD << 24) | (0xD << 28));

    // Reset I2C1
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    // 3. Cấu hình I2C1 Clock (Standard Mode 100kHz với PCLK1 = 36MHz)
    I2C1->CR2 |= 36;          // FREQ = 36 MHz
    I2C1->CCR = 180;          // Thô: 36MHz / (2 * 100kHz) = 180
    I2C1->TRISE = 37;         // 36MHz * 1us + 1 = 37

    // 4. Bật Module I2C1
    I2C1->CR1 |= I2C_CR1_PE;
}

void I2C1_Start(void) {
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
}

void I2C1_Stop(void) {
    I2C1->CR1 |= I2C_CR1_STOP;
}

void I2C1_WriteAddress(uint8_t address) {
    I2C1->DR = address;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR1;
    (void)I2C1->SR2; // Xóa cờ ADDR bằng cách đọc SR1 và SR2
}

void I2C1_WriteData(uint8_t data) {
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_BTF));
}

uint8_t I2C1_ReadAck(void) {
    I2C1->CR1 |= I2C_CR1_ACK;
    while (!(I2C1->SR1 & I2C_SR1_RXNE));
    return I2C1->DR;
}

uint8_t I2C1_ReadNack(void) {
    I2C1->CR1 &= ~I2C_CR1_ACK;
    I2C1_Stop();
    while (!(I2C1->SR1 & I2C_SR1_RXNE));
    return I2C1->DR;
}
