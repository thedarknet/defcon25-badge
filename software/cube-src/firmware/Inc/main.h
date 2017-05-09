/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define LED_STATUS_Pin GPIO_PIN_13
#define LED_STATUS_GPIO_Port GPIOC
#define LED7_Pin GPIO_PIN_14
#define LED7_GPIO_Port GPIOC
#define AUDIO3_Pin GPIO_PIN_15
#define AUDIO3_GPIO_Port GPIOC
#define LED8_Pin GPIO_PIN_0
#define LED8_GPIO_Port GPIOC
#define LED6_Pin GPIO_PIN_1
#define LED6_GPIO_Port GPIOC
#define LED5_Pin GPIO_PIN_2
#define LED5_GPIO_Port GPIOC
#define LED4_Pin GPIO_PIN_3
#define LED4_GPIO_Port GPIOC
#define RFM69_RESET_Pin GPIO_PIN_4
#define RFM69_RESET_GPIO_Port GPIOF
#define AUDIO_2_Pin GPIO_PIN_7
#define AUDIO_2_GPIO_Port GPIOA
#define RADIO_INTERRUPT_DIO0_EXTI4_Pin GPIO_PIN_4
#define RADIO_INTERRUPT_DIO0_EXTI4_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_10
#define LED3_GPIO_Port GPIOB
#define RADIO_INTERRUPT_DIO1_EXTI11_Pin GPIO_PIN_11
#define RADIO_INTERRUPT_DIO1_EXTI11_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_12
#define LCD_CS_GPIO_Port GPIOB
#define LCD_SPI2_SCK_Pin GPIO_PIN_13
#define LCD_SPI2_SCK_GPIO_Port GPIOB
#define LCD_DATA_CMD_Pin GPIO_PIN_14
#define LCD_DATA_CMD_GPIO_Port GPIOB
#define LCD_SPI2_MOSI_Pin GPIO_PIN_15
#define LCD_SPI2_MOSI_GPIO_Port GPIOB
#define LCD_BACK_LIT_Pin GPIO_PIN_6
#define LCD_BACK_LIT_GPIO_Port GPIOC
#define LCD_RESET_Pin GPIO_PIN_7
#define LCD_RESET_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_8
#define LED2_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_9
#define LED1_GPIO_Port GPIOC
#define AUDIO1_Pin GPIO_PIN_8
#define AUDIO1_GPIO_Port GPIOA
#define LED9_Pin GPIO_PIN_9
#define LED9_GPIO_Port GPIOA
#define LED10_Pin GPIO_PIN_10
#define LED10_GPIO_Port GPIOA
#define RADIO_SPI3_NSS_Pin GPIO_PIN_15
#define RADIO_SPI3_NSS_GPIO_Port GPIOA
#define RADIO_SPI3_SCK_Pin GPIO_PIN_10
#define RADIO_SPI3_SCK_GPIO_Port GPIOC
#define RADIO_SPI3_MISO_Pin GPIO_PIN_11
#define RADIO_SPI3_MISO_GPIO_Port GPIOC
#define RADIO_SPI3_MOSI_Pin GPIO_PIN_12
#define RADIO_SPI3_MOSI_GPIO_Port GPIOC
#define IR_RCV_Pin GPIO_PIN_2
#define IR_RCV_GPIO_Port GPIOD
#define SD_CARD_CS_Pin GPIO_PIN_5
#define SD_CARD_CS_GPIO_Port GPIOB
#define TIM_IR_CARRIER_FREQ_Pin GPIO_PIN_8
#define TIM_IR_CARRIER_FREQ_GPIO_Port GPIOB
#define IR_TX_Pin GPIO_PIN_9
#define IR_TX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
