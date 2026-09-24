#include "max7219.h"

void SPI1_Init(void) {
    // 1. Bật Clock cho GPIOA và SPI1
    RCC->APB2ENR |= (1 << 2) | (1 << 12); 

    // 2. Cấu hình chân GPIOA
    // Xóa cấu hình cũ của PA4 (CS), PA5 (SCK), PA7 (MOSI)
    GPIOA->CRL &= ~((0xFU << 16) | (0xFU << 20) | (0xFU << 28));
    
    // PA7 (MOSI) và PA5 (SCK): Alternate function push-pull, max speed 50MHz (CNF=10, MODE=11 -> 0xB)
    // PA4 (CS): General purpose push-pull, max speed 50MHz (CNF=00, MODE=11 -> 0x3)
    GPIOA->CRL |= ((0x3U << 16) | (0xBU << 20) | (0xBU << 28));
    
    // Kéo chân CS (PA4) lên mức cao trạng thái chờ
    GPIOA->BSRR = (1 << 4);

    // 3. Cấu hình SPI1: Master mode, Baudrate PCLK/16, CPOL=0, CPHA=0, 8-bit data, MSB first
    SPI1->CR1 = (1 << 2) | (3 << 3) | (1 << 9) | (1 << 8); // MSTR=1, BR=011 (fPCLK/16), SSM=1, SSI=1
    SPI1->CR1 |= (1 << 6); // SPE=1 (Enable SPI)
}

void MAX7219_Write(uint8_t address, uint8_t data) {
    GPIOA->BRR = (1 << 4); // Kéo CS xuống mức thấp để bắt đầu truyền
    
    // Gửi byte địa chỉ
    while(!(SPI1->SR & (1 << 1))); // Chờ cờ TXE (Transmit buffer empty)
    SPI1->DR = address;
    while(!(SPI1->SR & (1 << 0))); // Chờ cờ RXNE để hoàn tất xung clock
    (void)SPI1->DR;                // Đọc rỗng để xóa cờ RXNE
    
    // Gửi byte dữ liệu
    while(!(SPI1->SR & (1 << 1))); 
    SPI1->DR = data;
    while(!(SPI1->SR & (1 << 0))); 
    (void)SPI1->DR;
    
    while(SPI1->SR & (1 << 7));    // Chờ cờ BSY (Busy) xóa, đảm bảo SPI rảnh hoàn toàn
    
    GPIOA->BSRR = (1 << 4); // Kéo CS lên mức cao để chốt dữ liệu vào IC
}

void MAX7219_Init(void) {
    SPI1_Init();
    
    // Tắt Decode (0x00) để điều khiển từng LED đơn lẻ trên ma trận
    MAX7219_Write(MAX7219_REG_DECODE_MODE, 0x00);  
    
    MAX7219_Write(MAX7219_REG_INTENSITY, 0x03);    // Độ sáng mức thấp/trung bình (0x00 đến 0x0F)
    MAX7219_Write(MAX7219_REG_SCAN_LIMIT, 0x07);   // Quét hiển thị cả 8 hàng
    MAX7219_Write(MAX7219_REG_DISPLAY_TEST, 0x00); // Tắt chế độ test
    MAX7219_Write(MAX7219_REG_SHUTDOWN, 0x01);     // Bật chế độ hoạt động bình thường
    
    // Xóa trắng ma trận ban đầu
    for(int i = 1; i <= 8; i++) {
        MAX7219_Write(i, 0x00); 
    }
}

void MAX7219_DisplayPattern(uint8_t *pattern) {
    for(int i = 0; i < 8; i++) {
        MAX7219_Write(i + 1, pattern[i]); // Ghi từ hàng 1 đến hàng 8
    }
}
