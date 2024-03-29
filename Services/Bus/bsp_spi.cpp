#include "Bus/bsp_spi.h"
#include "main.h"



void SPI_DMA_init(SPI_WITH_DMA_t spi_t, uint32_t tx_buf, uint32_t rx_buf, uint16_t num)
{
    auto spi_h = spi_t.spiHandle;
    auto dma_rx = spi_t.rxDMAHandle;
    auto dma_tx = spi_t.txDMAHandle;

    SET_BIT(spi_h->Instance->CR2, SPI_CR2_TXDMAEN);
    SET_BIT(spi_h->Instance->CR2, SPI_CR2_RXDMAEN);

    __HAL_SPI_ENABLE(spi_h);


    //disable DMA
    //失效DMA
    __HAL_DMA_DISABLE(dma_rx);
    
    while(dma_rx->Instance->CR & DMA_SxCR_EN)
    {
        __HAL_DMA_DISABLE(dma_rx);
    }

    __HAL_DMA_CLEAR_FLAG(dma_rx, DMA_LISR_TCIF2);

    dma_rx->Instance->PAR = (uint32_t) & (SPI1->DR);
    //memory buffer 1
    //内存缓冲区1
    dma_rx->Instance->M0AR = (uint32_t)(rx_buf);
    //data length
    //数据长度
    __HAL_DMA_SET_COUNTER(dma_rx, num);

    __HAL_DMA_ENABLE_IT(dma_rx, DMA_IT_TC);


    //disable DMA
    //失效DMA
    __HAL_DMA_DISABLE(dma_tx);
    
    while(dma_tx->Instance->CR & DMA_SxCR_EN)
    {
        __HAL_DMA_DISABLE(dma_tx);
    }


    __HAL_DMA_CLEAR_FLAG(dma_tx, DMA_LISR_TCIF3);

    dma_tx->Instance->PAR = (uint32_t) & (SPI1->DR);
    //memory buffer 1
    //内存缓冲区1
    dma_tx->Instance->M0AR = (uint32_t)(tx_buf);
    //data length
    //数据长度
    __HAL_DMA_SET_COUNTER(dma_tx, num);


}

void SPI_DMA_enable(SPI_WITH_DMA_t spi_t, uint32_t tx_buf, uint32_t rx_buf, uint16_t ndtr)
{
    auto spi_h = spi_t.spiHandle;
    auto dma_rx = spi_t.rxDMAHandle;
    auto dma_tx = spi_t.txDMAHandle;

    //disable DMA
    //失效DMA
    __HAL_DMA_DISABLE(dma_rx);
    __HAL_DMA_DISABLE(dma_tx);
    while(dma_rx->Instance->CR & DMA_SxCR_EN)
    {
        __HAL_DMA_DISABLE(dma_rx);
    }
    while(dma_tx->Instance->CR & DMA_SxCR_EN)
    {
        __HAL_DMA_DISABLE(dma_tx);
    }
    //clear flag
    //清除标志位
    __HAL_DMA_CLEAR_FLAG (spi_h->hdmarx, __HAL_DMA_GET_TC_FLAG_INDEX(spi_h->hdmarx));
    __HAL_DMA_CLEAR_FLAG (spi_h->hdmarx, __HAL_DMA_GET_HT_FLAG_INDEX(spi_h->hdmarx));
    __HAL_DMA_CLEAR_FLAG (spi_h->hdmarx, __HAL_DMA_GET_TE_FLAG_INDEX(spi_h->hdmarx));
    __HAL_DMA_CLEAR_FLAG (spi_h->hdmarx, __HAL_DMA_GET_DME_FLAG_INDEX(spi_h->hdmarx));
    __HAL_DMA_CLEAR_FLAG (spi_h->hdmarx, __HAL_DMA_GET_FE_FLAG_INDEX(spi_h->hdmarx));

    __HAL_DMA_CLEAR_FLAG (spi_h->hdmatx, __HAL_DMA_GET_TC_FLAG_INDEX(spi_h->hdmatx));
    __HAL_DMA_CLEAR_FLAG (spi_h->hdmatx, __HAL_DMA_GET_HT_FLAG_INDEX(spi_h->hdmatx));
    __HAL_DMA_CLEAR_FLAG (spi_h->hdmatx, __HAL_DMA_GET_TE_FLAG_INDEX(spi_h->hdmatx));
    __HAL_DMA_CLEAR_FLAG (spi_h->hdmatx, __HAL_DMA_GET_DME_FLAG_INDEX(spi_h->hdmatx));
    __HAL_DMA_CLEAR_FLAG (spi_h->hdmatx, __HAL_DMA_GET_FE_FLAG_INDEX(spi_h->hdmatx));
    //set memory address
    //设置数据地址
    dma_rx->Instance->M0AR = rx_buf;
    dma_tx->Instance->M0AR = tx_buf;
    //set data length
    //设置数据长度
    __HAL_DMA_SET_COUNTER(dma_rx, ndtr);
    __HAL_DMA_SET_COUNTER(dma_tx, ndtr);
    //enable DMA
    //使能DMA
    __HAL_DMA_ENABLE(dma_rx);
    __HAL_DMA_ENABLE(dma_tx);
}




