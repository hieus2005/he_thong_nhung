#ifndef __DMA_H__
#define __DMA_H__

#include "stm32f10x.h"

/**
  * @brief Khởi tạo DMA1 Channel 4 chuyên trách cho USART1_TX
  */
void DMA1_USART1_TX_Init(void);

/**
  * @brief Kích hoạt DMA truyền dữ liệu từ bộ đệm RAM ra thanh ghi DR của USART1
  * @param buffer: Địa chỉ con trỏ chứa dữ liệu cần truyền
  * @param length: Số byte cần truyền
  */
void DMA1_USART1_TX_Send(char *buffer, uint16_t length);

#endif /* __DMA_H__ */
