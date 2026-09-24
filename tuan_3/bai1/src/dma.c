#include "dma.h"

void DMA1_USART1_TX_Init(void)
{
    // 1. Bật Clock cho DMA1 (Bit 0 trên thanh ghi RCC->AHBENR)
    RCC->AHBENR |= (1 << 0);

    // 2. Tắt DMA1 Channel 4 trước khi cấu hình (Bit 0 EN = 0)
    DMA1_Channel4->CCR &= ~(1 << 0);

    // 3. Đặt địa chỉ đích ngoại vi: Thanh ghi dữ liệu USART1->DR
    DMA1_Channel4->CPAR = (uint32_t)&(USART1->DR);

    // 4. Cấu hình thanh ghi CCR cho Channel 4:
    // - MINC (Bit 7 = 1): Tự động tăng địa chỉ bộ nhớ (RAM) sau mỗi byte
    // - DIR  (Bit 4 = 1): Đọc từ RAM ghi ra Ngoại vi (Memory to Peripheral)
    DMA1_Channel4->CCR = (1 << 7) | (1 << 4);
}

void DMA1_USART1_TX_Send(char *buffer, uint16_t length)
{
    // Tắt DMA Channel 4 để nạp tham số mới
    DMA1_Channel4->CCR &= ~(1 << 0);

    // Nạp địa chỉ RAM chứa chuỗi bản tin
    DMA1_Channel4->CMAR = (uint32_t)buffer;

    // Nạp số lượng byte cần phát
    DMA1_Channel4->CNDTR = length;

    // Xóa cờ báo hoàn tất truyền (CTCIF4 - Bit 13 trong IFCR)
    DMA1->IFCR |= (1 << 13);

    // Bật DMA Channel 4 để khởi chạy truyền dữ liệu ngay lập tức
    DMA1_Channel4->CCR |= (1 << 0);
}
