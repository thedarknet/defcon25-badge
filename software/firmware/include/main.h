/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  *
  * Copyright (c) 2016 STMicroelectronics International N.V. 
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
#define RADIO_SPI_SCK_Pin GPIO_PIN_5
#define RADIO_SPI_SCK_GPIO_Port GPIOA
#define RADIO_SPI_MISO_Pin GPIO_PIN_6
#define RADIO_SPI_MISO_GPIO_Port GPIOA
#define RADIO_SPI_MOSI_Pin GPIO_PIN_7
#define RADIO_SPI_MOSI_GPIO_Port GPIOA
#define RADIO_INTERRUPT_EXTI4_Pin GPIO_PIN_4
#define RADIO_INTERRUPT_EXTI4_GPIO_Port GPIOC
#define OLED_SPI_NSS_Pin GPIO_PIN_12
#define OLED_SPI_NSS_GPIO_Port GPIOB
#define OLED_SPI_SCK_Pin GPIO_PIN_13
#define OLED_SPI_SCK_GPIO_Port GPIOB
#define OLED_SPI_MISO_Pin GPIO_PIN_14
#define OLED_SPI_MISO_GPIO_Port GPIOB
#define OLED_SPI_MOSI_Pin GPIO_PIN_15
#define OLED_SPI_MOSI_GPIO_Port GPIOB
#define RADIO_SPI_NSS_Pin GPIO_PIN_15
#define RADIO_SPI_NSS_GPIO_Port GPIOA
#define TIM_IR_CARRIER_FREQ_Pin GPIO_PIN_5
#define TIM_IR_CARRIER_FREQ_GPIO_Port GPIOB
#define TIM_IR_MAIN_SIGNAL_Pin GPIO_PIN_8
#define TIM_IR_MAIN_SIGNAL_GPIO_Port GPIOB
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
