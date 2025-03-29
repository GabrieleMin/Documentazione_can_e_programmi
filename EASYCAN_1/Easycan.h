/* USER CODE BEGIN Header */

/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"  // HAL library for STM32F4

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* Struttura per la configurazione CAN (Structure for CAN configuration) */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

#define PARAMETRO_NON_NECESSARIO 0xFFFFFFFF   // Constant for unnecessary parameter
#define ID_NON_VALIDO 0xFFFF                 // Constant for invalid ID

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);  // Function prototype for handling errors

/* USER CODE BEGIN EFP */

/* Funzioni per la gestione del CAN (Functions for CAN management) */
void Easycan_Init(Easycan_Struct *easyStruct, CAN_HandleTypeDef *hcan, uint32_t RxFifo);   // Initialization of CAN
HAL_StatusTypeDef Easycan_SendMessage(Easycan_Struct *easyStruct, uint8_t aData[], uint32_t ResponseforthisID);  // Function to send CAN message
uint16_t Easycan_GetMessage(Easycan_Struct *easyStruct, uint8_t Data[]);  // Function to receive CAN message

#ifdef HAL_UART_MODULE_ENABLED
    void serialPrint(UART_HandleTypeDef *huart2, char *str);  // UART print function
    void serialPrintln(UART_HandleTypeDef huart2, char *str);  // UART print with newline
#endif

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

