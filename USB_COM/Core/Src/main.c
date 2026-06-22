/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f103xb.h"
#include "stm32f1xx_hal_gpio.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <string.h>
#include <usbd_cdc_if.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define USB_BUFLEN 128
#define DIGIT1 GPIO_PIN_3
#define DIGIT2 GPIO_PIN_2
#define DIGIT3 GPIO_PIN_1
#define DIGIT4 GPIO_PIN_0
#define SEGA GPIO_PIN_1 // port B
#define SEGB GPIO_PIN_0 // ~
#define SEGC GPIO_PIN_7
#define SEGD GPIO_PIN_6
#define SEGE GPIO_PIN_5
#define SEGF GPIO_PIN_4
#define SEGG GPIO_PIN_10 // ~
#define SEGP GPIO_PIN_11 // ~
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */
uint8_t usbTxBuff[USB_BUFLEN];
uint16_t usbTxBufLen;

uint8_t usbRxBuff[USB_BUFLEN];
uint16_t usbRXBufLen = 0;
uint8_t usbRXFlag =0;

typedef struct{
    GPIO_TypeDef *port;
    uint16_t Pin;
} PinData;

typedef struct{
  PinData* element;
  uint8_t size;
} list;

PinData LCDSegment1[] = {{GPIOB,SEGB}             ,{GPIOA,SEGC}};
PinData LCDSegment2[] = {{GPIOB,SEGA},{GPIOB,SEGB},             {GPIOA,SEGD},{GPIOA,SEGE},{GPIOB,SEGG}};
PinData LCDSegment3[] = {{GPIOB,SEGA},{GPIOB,SEGB},{GPIOA,SEGC},{GPIOA,SEGD},             {GPIOB,SEGG}};
PinData LCDSegment4[] = {             {GPIOB,SEGB},{GPIOA,SEGC},             {GPIOA,SEGF},{GPIOB,SEGG}};
PinData LCDSegment5[] = {{GPIOB,SEGA},             {GPIOA,SEGC},{GPIOA,SEGD},{GPIOA,SEGF},{GPIOB,SEGG}};
PinData LCDSegment6[] = {{GPIOB,SEGA},             {GPIOA,SEGC},{GPIOA,SEGD},{GPIOA,SEGE},{GPIOA,SEGF},{GPIOB,SEGG}};
PinData LCDSegment7[] = {{GPIOB,SEGA},{GPIOB,SEGB},{GPIOA,SEGC}};
PinData LCDSegment8[] = {{GPIOB,SEGA},{GPIOB,SEGB},{GPIOA,SEGC},{GPIOA,SEGD},{GPIOA,SEGE},{GPIOA,SEGF},{GPIOB,SEGG}};
PinData LCDSegment9[] = {{GPIOB,SEGA},{GPIOB,SEGB},{GPIOA,SEGC},{GPIOA,SEGD},             {GPIOA,SEGF},{GPIOB,SEGG}};
PinData LCDSegment0[] = {{GPIOB,SEGA},{GPIOB,SEGB},{GPIOA,SEGC},{GPIOA,SEGD},{GPIOA,SEGE},{GPIOA,SEGF}};


list LCDList[] = {{LCDSegment0, 7},
                  {LCDSegment1, 2},
                  {LCDSegment2, 5},
                  {LCDSegment3, 5},
                  {LCDSegment4, 4},
                  {LCDSegment5, 5},
                  {LCDSegment6, 6},
                  {LCDSegment7, 3},
                  {LCDSegment8, 7},
                  {LCDSegment9, 6}};

  
/* USER CODE END PV */


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */
void LCD_LEDConfig(uint8_t Value);
void LCDWrite(uint16_t DigitPosition, uint8_t WriteValue);
void DelayUS(TIM_HandleTypeDef *tim , uint8_t time);

void LCDUpdateBuf();
void LCDWriteRXBuf(uint8_t *Buf, uint32_t length);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void USB_RXCallback(uint8_t*Buf,uint32_t *Len){
  memset(usbRxBuff,0,sizeof(usbRxBuff)); // reset buffer
  memcpy(usbRxBuff,Buf,*Len); // cpy buf to Rx buffer 
  
  usbRXBufLen = *Len; // set length
  usbRXFlag =1; // set flag
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
  MX_USB_DEVICE_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim1);
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    LCDWriteRXBuf(usbRxBuff,usbRXBufLen);

    if (usbRXFlag && usbRXBufLen){  // while ready and buffer has content
      usbTxBufLen = snprintf((char *)usbTxBuff, USB_BUFLEN, "[%s]:%d \r\n",usbRxBuff,usbRXBufLen);
      // the send buffer will copy the RX data, under the format "bytes received: length : content"
      CDC_Transmit_FS(usbTxBuff, USB_BUFLEN);

      memset(usbTxBuff,0,sizeof(usbTxBuff)); // reset tx buffer
      usbRXFlag = 0;
    } 
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV8;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 11;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3
                           PA4 PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB10 PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void LCDWrite(uint16_t DigitPosition, uint8_t WriteValue){
    HAL_GPIO_WritePin(GPIOA,DigitPosition,GPIO_PIN_RESET); // enables the LED 
    LCD_LEDConfig(WriteValue); // draws the number
    HAL_GPIO_WritePin(GPIOA,DigitPosition,GPIO_PIN_SET); // disables the LED
}
void LCD_LEDConfig(uint8_t Value){

  for (int i = 0; i < LCDList[Value].size; i++){
    HAL_GPIO_WritePin(LCDList[Value].element[i].port,LCDList[Value].element[i].Pin,GPIO_PIN_SET);
    DelayUS(&htim1, 10);
    HAL_GPIO_WritePin(LCDList[Value].element[i].port,LCDList[Value].element[i].Pin,GPIO_PIN_RESET);
  }
}



void LCDWriteRXBuf(uint8_t *Buf, uint32_t length){

  switch (length){
  
    case 1:
    LCDWrite(DIGIT4, Buf[0] - 48);
    break;
    case 2:
    LCDWrite(DIGIT3, Buf[0] - 48);
    LCDWrite(DIGIT4, Buf[1] - 48);
    break;
    case 3:
    LCDWrite(DIGIT2, Buf[0] - 48);
    LCDWrite(DIGIT3, Buf[1] - 48);
    LCDWrite(DIGIT4, Buf[2] - 48);
    break;
    default:
    LCDWrite(DIGIT1, 0);
}

}
void DelayUS(TIM_HandleTypeDef *tim , uint8_t time){

    __HAL_TIM_SET_COUNTER(tim, 0);
    while(__HAL_TIM_GET_COUNTER(tim) < 1){}
}
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
