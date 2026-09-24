#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x.h"

void I2C1_Init(void);
void I2C1_Start(void);
void I2C1_Stop(void);
void I2C1_WriteAddress(uint8_t address);
void I2C1_WriteData(uint8_t data);
uint8_t I2C1_ReadAck(void);
uint8_t I2C1_ReadNack(void);

#endif
