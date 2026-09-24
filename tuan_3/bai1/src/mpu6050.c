#include "mpu6050.h"
#include "i2c.h"

uint8_t MPU6050_Init(void) {
    I2C1_Init();

    // Kiểm tra kết nối MPU6050 qua thanh ghi WHO_AM_I (0x75)
    I2C1_Start();
    I2C1_WriteAddress(MPU6050_ADDR);
    I2C1_WriteData(MPU6050_RA_WHO_AM_I);
    I2C1_Start();
    I2C1_WriteAddress(MPU6050_ADDR | 0x01); // Mode Read
    uint8_t check = I2C1_ReadNack();

    if (check == 0x68) { // MPU6050 trả về 0x68 nếu giao tiếp I2C đúng
        // Đánh thức MPU6050 (Thoát khỏi chế độ Sleep)
        I2C1_Start();
        I2C1_WriteAddress(MPU6050_ADDR);
        I2C1_WriteData(MPU6050_RA_PWR_MGMT_1);
        I2C1_WriteData(0x00);
        I2C1_Stop();
        return 0; // Thành công
    }
    return 1; // Thất bại
}

void MPU6050_Read_All(MPU6050_Data_t *DataStruct) {
    uint8_t buffer[14];

    // Đọc liên tiếp 14 byte từ ACCEL_XOUT_H (0x3B) đến GYRO_ZOUT_L (0x44)
    I2C1_Start();
    I2C1_WriteAddress(MPU6050_ADDR);
    I2C1_WriteData(MPU6050_RA_ACCEL_XOUT_H);
    
    I2C1_Start();
    I2C1_WriteAddress(MPU6050_ADDR | 0x01);

    for (int i = 0; i < 13; i++) {
        buffer[i] = I2C1_ReadAck();
    }
    buffer[13] = I2C1_ReadNack();

    // Ghép 2 byte cao/thấp cho từng trục
    DataStruct->Accel_X = (int16_t)(buffer[0] << 8 | buffer[1]);
    DataStruct->Accel_Y = (int16_t)(buffer[2] << 8 | buffer[3]);
    DataStruct->Accel_Z = (int16_t)(buffer[4] << 8 | buffer[5]);
    // buffer[6], buffer[7] là Temp
    DataStruct->Gyro_X  = (int16_t)(buffer[8] << 8 | buffer[9]);
    DataStruct->Gyro_Y  = (int16_t)(buffer[10] << 8 | buffer[11]);
    DataStruct->Gyro_Z  = (int16_t)(buffer[12] << 8 | buffer[13]);
}
