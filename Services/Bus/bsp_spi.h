
#ifndef BSP_SPI_H
#define BSP_SPI_H
#include "main.h"
#include "ProjectConfig.h"


extern void SPI_DMA_init(SPI_WITH_DMA_t spi_t, uint32_t tx_buf, uint32_t rx_buf, uint16_t num);
extern void SPI_DMA_enable(SPI_WITH_DMA_t spi_t, uint32_t tx_buf, uint32_t rx_buf, uint16_t ndtr);

#endif
