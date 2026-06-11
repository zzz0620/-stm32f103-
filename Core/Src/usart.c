/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 230400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_USART1_ENABLE();

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA1_Channel4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 DMA Init */
    /* USART3_RX Init */
    hdma_usart3_rx.Instance = DMA1_Channel3;
    hdma_usart3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_rx.Init.Mode = DMA_NORMAL;
    hdma_usart3_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    if (HAL_DMA_Init(&hdma_usart3_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart3_rx);

    /* USART3_TX Init */
    hdma_usart3_tx.Instance = DMA1_Channel2;
    hdma_usart3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_tx.Init.Mode = DMA_NORMAL;
    hdma_usart3_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    if (HAL_DMA_Init(&hdma_usart3_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart3_tx);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

    /* USART3 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void usart1_init(void)  
{  
    // Ê¹ÄÜ USART1 ¿ÕÏÐÖÐ¶Ï  
    // Õâ½«ÔÊÐí USART1 ÔÚ¿ÕÏÐ×´Ì¬Ê±²úÉúÖÐ¶Ï  
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);  
}  

void usart1_tx_dma_init(void)  
{  
    // Ê¹ÄÜUSART1µÄDMA·¢ËÍÇëÇó£¬ÓÃÓÚÊý¾Ý´«Êä  
    // È·±£USART1¿ÉÒÔÊ¹ÓÃDMA½øÐÐ·¢ËÍ  
    SET_BIT(huart1.Instance->CR3, USART_CR3_DMAT);  

    // ½ûÓÃDMA´«ÊäÒÔ½øÐÐÅäÖÃ  
    // ÔÚÐÞ¸ÄDMAÉèÖÃÖ®Ç°£¬Ê×ÏÈÈ·±£DMAÒÑ±»½ûÓÃ  
    do  
    {  
        __HAL_DMA_DISABLE(&hdma_usart1_tx);  
    } while (hdma_usart1_tx.Instance->CCR & DMA_CCR_EN); // µÈ´ýDMAÍ¨µÀ±»½ûÓÃ  

    // ÉèÖÃDMAÍâÉèµØÖ·¼Ä´æÆ÷CPAR£¬Ö¸ÏòUSART1µÄÊý¾Ý¼Ä´æÆ÷DR  
    hdma_usart1_tx.Instance->CPAR = (uint32_t) & (USART1->DR);  
    
    // ÉèÖÃDMAÄÚ´æµØÖ·¼Ä´æÆ÷CMAR£¬Ö¸ÏòÒª·¢ËÍµÄÊý¾Ý»º³åÇø  
    hdma_usart1_tx.Instance->CMAR = (uint32_t)(NULL); // ÔÝÉèÎª¿Õ£¬Êµ¼ÊÊ¹ÓÃÊ±»á¸üÐÂÎªÓÐÐ§»º³åÇø  

    // ÉèÖÃDMAÊý¾Ý´«ÊäÊý¼Ä´æÆ÷CNDTR£¬³õÊ¼»¯Îª0  
    hdma_usart1_tx.Instance->CNDTR = 0; // Ö¸¶¨Òª·¢ËÍµÄÊý¾Ý×Ö½ÚÊý£¬³õÊ¼»¯Îª0  
}  

void usart1_rx_dma_init(uint8_t *rx_buf, uint16_t dma_buf_num)  
{  
    // Ê¹ÄÜUSART1µÄDMA½ÓÊÕÇëÇó£¬ÔÊÐíÊ¹ÓÃDMA½ÓÊÕÊý¾Ý  
    SET_BIT(huart1.Instance->CR3, USART_CR3_DMAR);  

    // ½ûÓÃDMA½ÓÊÕÍ¨µÀÒÔ½øÐÐÅäÖÃ  
    do  
    {  
        __HAL_DMA_DISABLE(&hdma_usart1_rx);  
    } while (hdma_usart1_rx.Instance->CCR & DMA_CCR_EN); // µÈ´ýDMAÍ¨µÀ±»½ûÓÃ  

    // Çå³ýDMAÖÐ¶Ï±êÖ¾£¨´«ÊäÍê³É±êÖ¾TC£©£¬ÒÔ×¼±¸½ÓÊÕÐÂÊý¾Ý  
    __HAL_DMA_CLEAR_FLAG(&hdma_usart1_rx, DMA_ISR_TCIF5);  

    // ÉèÖÃDMAÍâÉèµØÖ·¼Ä´æÆ÷CPAR£¬Ö¸ÏòUSART1µÄÊý¾Ý¼Ä´æÆ÷DR  
    hdma_usart1_rx.Instance->CPAR = (uint32_t) & (USART1->DR);  
    
    // ÉèÖÃDMAÄÚ´æ»º³åÇøCMAR£¬Ö¸Ïò½ÓÊÕ»º³åÇørx_buf  
    hdma_usart1_rx.Instance->CMAR = (uint32_t)(rx_buf);  

    // ÉèÖÃDMAµÄÊý¾Ý³¤¶È£¬¼´Òª½ÓÊÕµÄ×Ö½ÚÊý  
    hdma_usart1_rx.Instance->CNDTR = (uint16_t)(dma_buf_num); // Ö¸¶¨Òª½ÓÊÕµÄ×Ö½ÚÊý  

    // ÆôÓÃDMA½ÓÊÕÍ¨µÀ£¬ÔÊÐí¿ªÊ¼Êý¾Ý½ÓÊÕ  
    __HAL_DMA_ENABLE(&hdma_usart1_rx);  
}

void usart1_tx_dma_enable(uint8_t *data, uint16_t len)
{
    // ½ûÓÃDMA´«ÊäÒÔ½øÐÐÅäÖÃ
    do
		{
				__HAL_DMA_DISABLE(&hdma_usart1_tx);
    }while(hdma_usart1_tx.Instance->CCR & DMA_CCR_EN);// µÈ´ýDMAÍ¨µÀ±»½ûÓÃ

    __HAL_DMA_CLEAR_FLAG(&hdma_usart1_tx, DMA_ISR_TCIF4);

    hdma_usart1_tx.Instance->CMAR = (uint32_t)(data);
    hdma_usart1_tx.Instance->CNDTR = (uint16_t)(len);

    __HAL_DMA_ENABLE(&hdma_usart1_tx);
}

void usart3_init(void)
{
    // Ê¹ÄÜ USART3 ¿ÕÏÐÖÐ¶Ï  
    // Õâ½«ÔÊÐí USART3 ÔÚ¿ÕÏÐ×´Ì¬Ê±²úÉúÖÐ¶Ï
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
}

void usart3_tx_dma_init(void)  
{  
    // Ê¹ÄÜUSART3µÄDMA´«ÊäÇëÇó£¬ÓÃÓÚ·¢ËÍÊý¾Ý  
    SET_BIT(huart3.Instance->CR3, USART_CR3_DMAT);  

    // ½ûÓÃDMA´«ÊäÒÔ°²È«½øÐÐÉèÖÃ  
    do  
    {  
        __HAL_DMA_DISABLE(&hdma_usart3_tx);  
    } while (hdma_usart3_tx.Instance->CCR & DMA_CCR_EN); // µÈ´ýDMAÍ¨µÀ±»½ûÓÃ  

    // ÉèÖÃDMAÍâÉèµØÖ·¼Ä´æÆ÷CPAR£¬Ö¸ÏòUSART3µÄÊý¾Ý¼Ä´æÆ÷DR  
    hdma_usart3_tx.Instance->CPAR = (uint32_t) &(USART3->DR);  
    
    // ÉèÖÃDMAÄÚ´æµØÖ·¼Ä´æÆ÷CMAR£¬Ö¸Ïò·¢ËÍ»º³åÇø£¨´Ë´¦Ô¤ÉèÎªNULL£¬ÐèÔÚÊµ¼Ê·¢ËÍÊ±¸üÐÂ£©  
    hdma_usart3_tx.Instance->CMAR = (uint32_t)(NULL);  
    
    // ÉèÖÃDMAÊý¾Ý´«ÊäÊý¼Ä´æÆ÷CNDTR£¬³õÊ¼»¯Îª0  
    hdma_usart3_tx.Instance->CNDTR = 0;  
}  

void usart3_rx_dma_init(uint8_t *rx_buf, uint16_t dma_buf_num)  
{  
    // Ê¹ÄÜUSART3µÄDMA´«ÊäÇëÇó£¬ÓÃÓÚ½ÓÊÕÊý¾Ý  
    SET_BIT(huart3.Instance->CR3, USART_CR3_DMAR);  

    // ½ûÓÃDMA´«ÊäÒÔ½øÐÐÅäÖÃ  
    do  
    {  
        __HAL_DMA_DISABLE(&hdma_usart3_rx);  
    } while (hdma_usart3_rx.Instance->CCR & DMA_CCR_EN); // µÈ´ýDMAÍ¨µÀ±»½ûÓÃ  

    // Çå³ýDMAÖÐ¶Ï±êÖ¾£¨´«ÊäÍê³É±êÖ¾TC£©  
    __HAL_DMA_CLEAR_FLAG(&hdma_usart3_rx, DMA_ISR_TCIF3);  
    
    // ÉèÖÃDMAÍâÉèµØÖ·¼Ä´æÆ÷CPAR£¬Ö¸ÏòUSART3µÄÊý¾Ý¼Ä´æÆ÷DR  
    hdma_usart3_rx.Instance->CPAR = (uint32_t) &(USART3->DR);  
    
    // ÉèÖÃDMAÄÚ´æ»º³åÇøCMAR£¬Ö¸Ïò½ÓÊÕ»º³åÇø  
    hdma_usart3_rx.Instance->CMAR = (uint32_t)(rx_buf);  

    // ÉèÖÃDMAµÄÊý¾Ý³¤¶È£¬¼´Òª½ÓÊÕµÄ×Ö½ÚÊý  
    hdma_usart3_rx.Instance->CNDTR = (uint16_t)(dma_buf_num);

    // ÆôÓÃDMA½ÓÊÕ£¬ÔÊÐí¿ªÊ¼´«Êä  
    __HAL_DMA_ENABLE(&hdma_usart3_rx);  
}


void usart3_tx_dma_enable(uint8_t *data, uint16_t len)
{
    // disable DMA
    // Ê§Ð§DMA
    do
		{
				__HAL_DMA_DISABLE(&hdma_usart3_tx);
    }while(hdma_usart3_tx.Instance->CCR & DMA_CCR_EN);

    __HAL_DMA_CLEAR_FLAG(&hdma_usart3_tx, DMA_ISR_TCIF2);

    hdma_usart3_tx.Instance->CMAR = (uint32_t)(data);
    hdma_usart3_tx.Instance->CNDTR = (uint16_t)(len);

    __HAL_DMA_ENABLE(&hdma_usart3_tx);
}

/* USER CODE END 1 */
