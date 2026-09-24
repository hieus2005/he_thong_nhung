#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f10x.h"

#define MPU6050_ADDR         0xD0 // Địa chỉ I2C của MPU6050 (CSG = GND -> 0x68 << 1)

// Các thanh ghi chính
#define MPU6050_RA_PWR_MGMT_1  0x6B
#define MPU6050_RA_ACCEL_XOUT_H 0x3B
#define MPU6050_RA_GYRO_XOUT_H  0x43
#define MPU6050_RA_WHO_AM_I    0x75

// Cấu trúc lưu dữ liệu IMU
typedef struct {
    int16_t Accel_X;
    int16_t Accel_Y;
    int16_t Accel_Z;
    int16_t Gyro_X;
    int16_t Gyro_Y;
    int16_t Gyro_Z;
} MPU6050_Data_t;

uint8_t MPU6050_Init(void);
void MPU6050_Read_All(MPU6050_Data_t *DataStruct);

#endif
