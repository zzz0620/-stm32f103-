/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "frame.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "esp_serve.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define ENABLE_TX 1

#define NO_FORCE_TX 0
#define server_data_len 128
#define LEN 21
bool TX_MODE = ENABLE_TX;          /*使能转发功能*/
#define FLASH_SAVE_ADDR 0x0800FC00 // 最后一页地址
enum mode
{
  SET_TEAM,
  SET_ID,
  SET_ROBOT,
  NORMAL_MODE,
};
enum mode change_mode = NORMAL_MODE;
frame_Frame Server_data;
volatile concept_Team team_color = concept_Team_None;                       // a8亮蓝 灭红
volatile concept_DetailRobot robot = concept_DetailRobot_Infantry; // a11亮工程 灭步兵
int last_team;
concept_DetailRobot last_robot;
int robot_id = 1; // 机器人ID
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
static volatile uint8_t uart1_rx_buf[LEN] = {0}; // 消息接收缓冲区
static volatile uint8_t uart3_rx_buf[server_data_len] = {0};

void Save_Config_To_Flash(concept_Team team, concept_DetailRobot robot)
{
  __disable_irq();    /* 关闭中断 */
  HAL_FLASH_Unlock(); /* 解锁Flash*/
  FLASH_EraseInitTypeDef eraseInitStruct;
  uint32_t pageError = 0;
  eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  eraseInitStruct.PageAddress = FLASH_SAVE_ADDR;
  eraseInitStruct.NbPages = 1;

  if (HAL_FLASHEx_Erase(&eraseInitStruct, &pageError) != HAL_OK)
  {
    HAL_FLASH_Lock();
    __enable_irq();
    return;
  }
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_SAVE_ADDR, team) != HAL_OK)
  {
    HAL_FLASH_Lock();
    __enable_irq();
    return;
  }
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_SAVE_ADDR + 4, robot) != HAL_OK)
  {
    HAL_FLASH_Lock();
    __enable_irq();
    return;
  }
  HAL_FLASH_Lock(); /* 锁住Flash */
  __enable_irq();   /*使能中断 */
}
void Load_Config_From_Flash(concept_Team *team, concept_DetailRobot *robot)
{
  *team = (concept_Team)(*(uint32_t *)FLASH_SAVE_ADDR);
  *robot = (concept_DetailRobot)(*(uint32_t *)(FLASH_SAVE_ADDR + 4));
}
bool uart3_dma_busy = false;
void UartTx_Task(void const *argument)
{
  while (1)
  {
    Robot_Infantry_Client_Transmit(team_color, robot); /*发送上位机*/
                                                       // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
                                                       // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
                                                       // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
    vTaskDelay(50);
  }
}

int flag_team = 0;
int flag_robot = 0;
void set_team(void)
{
  if (flag_team % 2 == 0)
  {
    team_color = concept_Team_Red;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    Save_Config_To_Flash(team_color, robot); // 保存到Flash
    // 设置红队
  }
  else
  {
    team_color = concept_Team_Blue;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
    Save_Config_To_Flash(team_color, robot); // 保存到Flash
    // 设置蓝队
  }

  // 设置队伍颜色代码
}
void Esp_Handel(frame_Frame data)
{
  if (robot == concept_DetailRobot_Infantry)
  {
    /*工程*/
    if (data.content.robot_command.infantry_powered == true)
    {
      TX_MODE = ENABLE_TX;
    }
    else if (data.content.robot_command.infantry_powered == false)
    {
      TX_MODE = NO_FORCE_TX;
    }
  }

  else
  {
    /*步兵*/
    if (data.content.robot_command.infantry_enabled == true)
    {
      TX_MODE = ENABLE_TX;
    }
    else if (data.content.robot_command.infantry_enabled == false)
    {
      TX_MODE = NO_FORCE_TX;
    }
  }
}
void normal_mode(void)
{
  // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
  //  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);

  if (team_color == concept_Team_Red)
  {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
  }
  else
  {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
  }

  // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
}
void set_robot(void)
{
  if (flag_robot % 2 == 0)
  {
    robot = concept_DetailRobot_Infantry;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
    Save_Config_To_Flash(team_color, robot); // 仅状态变化时写入Flash
    // 设置步兵
  }
  else if (flag_robot % 2 != 0)
  {
    robot = concept_DetailRobot_Engineer;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
    Save_Config_To_Flash(team_color, robot); // 仅状态变化时写入Flash
    // 设置工程
  }
}
void change_data(void)
{
  switch (change_mode)
  {
  case NORMAL_MODE:
    normal_mode();
    break;
  case SET_TEAM:
    set_team();
    break;
  case SET_ROBOT:
    set_robot();
    break;

  default:
    break;
  }
}

uint8_t count_quit = 0;
uint8_t count_set_team = 0;
uint8_t count_set_robot = 0;
void Change_Mode_Task(void const *argument)
{

  while (1)
  {
    //    HAL_UART_Receive(&huart3, uart3_rx_buf, server_data_len, 1000); // 阻塞接收10字节，超时1000ms
    //    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9,0);
    //		Server_data = Robot_Infantry_Client_UnPack((uint8_t *)uart3_rx_buf);
    //    Esp_Handel(Server_data);
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == GPIO_PIN_RESET && count_set_team < 200)
    {
      count_set_team++;
    }
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET && count_set_robot < 200)
    {
      count_set_robot++;
    }
    if (count_set_team >= 200)
    {
      change_mode = SET_TEAM;
      count_quit++;
      if (count_quit > 200)
      {
        change_mode = NORMAL_MODE;
        count_set_team = 0;
        count_set_robot = 0;
        count_quit = 0;
      }
    }
    if (count_set_robot >= 200)
    {
      change_mode = SET_ROBOT;
      count_quit++;
      if (count_quit > 200)
      {
        change_mode = NORMAL_MODE;
        count_set_team = 0;
        count_set_robot = 0;
        count_quit = 0;
      }
    }
    change_data();
    vTaskDelay(1);
  }
}
void INIT(void)
{
  HAL_StatusTypeDef ret;

  Load_Config_From_Flash(&team_color, &robot);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);

  HAL_UART_Receive_DMA(&huart1, uart1_rx_buf, (uint16_t)LEN);
  // HAL_UART_Receive_DMA(&huart3, uart3_rx_buf, (uint16_t)server_data_len);
  HAL_UARTEx_ReceiveToIdle_DMA(&huart3, uart3_rx_buf, server_data_len);
}
bool DMA_Busy = false;
uint8_t memory_init[LEN] = {0};
typedef enum
{
  POWER_NO_ENABLE,        // 下电
  POWER_ON_INIT = 0,      // 上电初始化
  INIT_READY,             // 初始化完成，准备就绪
  RESTORE_PREVIOUS_STATE, // 恢复下电时摩擦轮状态 因为小孩肯定不写重新上电过程 所以摩擦轮会提速很快 容易烧板子
  NORMAL_RUN              // 正常运行
} SystemBootStage;
SystemBootStage System_Station = NORMAL_RUN;
uint16_t Shoot_Speed = 500;
uint32_t Shoot_Start = 0;
int temp = 1;
/*UART的DMA回调*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == &huart1)
  {
    DMA_Busy = false;                       // 发送完成后清除 DMA_Busy
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10); // 可视化发送完成
    // 不要在这里重启 DMA 接收，接收在 Rx 回调中处理
  }
}
uint16_t CHN_PWM_Fre[8] = {0};
uint8_t last_tx_buf[LEN] = {0};
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  static uint8_t tx_buf[LEN]; // 使用独立的发送缓冲区
  if (huart == &huart1)
  {
    // 如果正在发送，直接重启接收以保证链路不中断
    if (DMA_Busy)
    {
      HAL_UART_Receive_DMA(&huart1, (uint8_t *)uart1_rx_buf, (uint16_t)LEN);
      return;
    }

    HAL_StatusTypeDef ret = HAL_OK;
    if (uart1_rx_buf[0] == 0xAB &&  /*帧头*/
        uart1_rx_buf[1] == 0xBC &&  /*帧头*/
        uart1_rx_buf[2] == 0xAA &&  /*初始化命令*/
        uart1_rx_buf[19] == 0xCD && /*帧尾*/
        uart1_rx_buf[20] == 0xDE)   /*帧尾*/
    {
      memcpy((void *)memory_init, (const void *)uart1_rx_buf, LEN); /*记录初始化顺序*/
      CHN_PWM_Fre[0] = (uint16_t)(memory_init[3] << 8 | memory_init[4]);
      CHN_PWM_Fre[1] = (uint16_t)(memory_init[5] << 8 | memory_init[6]);
      CHN_PWM_Fre[2] = (uint16_t)(memory_init[7] << 8 | memory_init[8]);
      CHN_PWM_Fre[3] = (uint16_t)(memory_init[9] << 8 | memory_init[10]);
      CHN_PWM_Fre[4] = (uint16_t)(memory_init[11] << 8 | memory_init[12]);
      CHN_PWM_Fre[5] = (uint16_t)(memory_init[13] << 8 | memory_init[14]);
      CHN_PWM_Fre[6] = (uint16_t)(memory_init[15] << 8 | memory_init[16]);
      CHN_PWM_Fre[7] = (uint16_t)(memory_init[17] << 8 | memory_init[18]);
    }
    // 根据 TX_MODE 决定转发逻辑
    if (TX_MODE == ENABLE_TX)
    {
      // 检查数据是否符合协议要求
      if (uart1_rx_buf[0] == 0xAB && uart1_rx_buf[1] == 0xBC)
      {
        memcpy(tx_buf, (const uint8_t *)uart1_rx_buf, LEN); // 去掉 volatile 修饰符
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);              // 可视化转发
        ret = HAL_UART_Transmit_DMA(&huart1, tx_buf, LEN);
        if (ret == HAL_OK)
        {
          DMA_Busy = true; // 标记 DMA 正在发送
        }
      }
      if (robot == concept_DetailRobot_Engineer)
      {
      }
      else if (robot == concept_DetailRobot_Infantry)
      {
      }

      memcpy((void *)last_tx_buf, (const void *)tx_buf, LEN); /*记录初始化顺序*/
    }
    else if (TX_MODE == NO_FORCE_TX)
    {

      memcpy(tx_buf, (const uint8_t *)uart1_rx_buf, LEN); // 去掉 volatile 修饰符
      // 修改数据并转发
      for (uint8_t ch = 0; ch < 8; ++ch)
      {
        /*判断机器人类型决定断电逻辑*/
        if (robot == concept_DetailRobot_Infantry)
        {
          if (CHN_PWM_Fre[ch] == 10000) /*电机速度给0*/
          {
            tx_buf[3 + ch * 2] = 0x00;     // 高字节
            tx_buf[3 + ch * 2 + 1] = 0x00; // 低字节
          }
          if (CHN_PWM_Fre[ch] == 50 && ch != 3 && ch != 4) /*3 4是摩擦轮 摩擦轮(频率50)不断电 不进if 剩下的为上一次占空比*/
          {
            tx_buf[3 + ch * 2] = last_tx_buf[3 + ch * 2];
            tx_buf[3 + ch * 2 + 1] = last_tx_buf[3 + ch * 2 + 1];
          }
        }
        else if (robot == concept_DetailRobot_Engineer)
        {
          if (CHN_PWM_Fre[ch] == 10000)
          {
            tx_buf[3 + ch * 2] = 0x00;     // 高字节
            tx_buf[3 + ch * 2 + 1] = 0x00; // 低字节
          }
          if (CHN_PWM_Fre[ch] == 50)
          {
            tx_buf[3 + ch * 2] = last_tx_buf[3 + ch * 2];
            tx_buf[3 + ch * 2 + 1] = last_tx_buf[3 + ch * 2 +1];
          }
        }
      }
      ret = HAL_UART_Transmit_DMA(&huart1, tx_buf, LEN);
      if (ret == HAL_OK)
      {
        DMA_Busy = true; // 标记 DMA 正在发送
      }
      memcpy((void *)last_tx_buf, (const void *)tx_buf, LEN); /*记录上一次数据*/
    }

    // 如果发送失败，记录错误状态（可选）
    if (ret != HAL_OK)
    {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); // 发送失败指示
    }
    else
    {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // 发送成功指示
    }

    // 始终重启接收，防止接收链路断开
    HAL_UART_Receive_DMA(&huart1, (uint8_t *)uart1_rx_buf, (uint16_t)LEN);
  }
}

// 错误回调里重启DMA
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart == &huart1)
  {
    DMA_Busy = false;
    HAL_UART_Receive_DMA(&huart1, (uint8_t *)uart1_rx_buf, (uint16_t)LEN);
  }
  else if (huart == &huart3)
  {
    HAL_UART_Receive_DMA(&huart3, (uint8_t *)uart3_rx_buf, (uint16_t)server_data_len);
  }
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart == &huart3)
  {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);                               // 每收到一包数据闪灯
    Server_data = Robot_Infantry_Client_UnPack((uint8_t *)uart3_rx_buf); /*解包*/
    Esp_Handel(Server_data);
    if (TX_MODE == NO_FORCE_TX)
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET); // 正常工作状态
    else
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);                // 正常工作状态
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, uart3_rx_buf, server_data_len); // 继续接收
  }
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(10000);
  INIT();

  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
