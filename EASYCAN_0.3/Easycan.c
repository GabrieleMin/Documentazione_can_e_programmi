#ifndef EASYCAN_H
#define EASYCAN_H


#include "stm32f4xx_hal.h"  // Includi la libreria appropriata per il tuo microcontrollore
#include <stdlib.h>

#define PARAMETRO_NON_NECESSARIO 0xFFFFFFFF
#define ID_NON_VALIDO 0xFFFF

/* Struttura per la configurazione CAN */
typedef struct {
    CAN_TxHeaderTypeDef TxHeader;
    CAN_RxHeaderTypeDef RxHeader;
    uint32_t pTxMailbox;
    CAN_HandleTypeDef *hcan;
    uint32_t RxFifo;
    uint32_t canTimeout;
} Easycan_Struct;

/* Funzione di inizializzazione della configurazione CAN */
static inline void Easycan_init(Easycan_Struct *easyStruct, CAN_HandleTypeDef *hcan, uint32_t RxFifo) {
    easyStruct->hcan = hcan;
    easyStruct->RxFifo = RxFifo;
    easyStruct->canTimeout = 10;
    
    // Inizializzazione dei campi della struttura TxHeader
    easyStruct->TxHeader.DLC = 8;
    easyStruct->TxHeader.ExtId = 0;
    easyStruct->TxHeader.IDE = CAN_ID_STD;
    easyStruct->TxHeader.RTR = CAN_RTR_DATA;
    easyStruct->TxHeader.StdId = 0;
    easyStruct->TxHeader.TransmitGlobalTime = DISABLE;
}

/* Funzione per inviare un messaggio CAN */
static inline HAL_StatusTypeDef Easycan_SendMessage(Easycan_Struct *easyStruct, uint8_t aData[], uint32_t ResponseforthisID) {
    HAL_StatusTypeDef mes;

    if (ResponseforthisID != PARAMETRO_NON_NECESSARIO) {
        uint32_t OLDStdId = easyStruct->TxHeader.StdId;
        easyStruct->TxHeader.StdId = ResponseforthisID;
        mes = HAL_CAN_AddTxMessage(easyStruct->hcan, &easyStruct->TxHeader, aData, &easyStruct->pTxMailbox);
        easyStruct->TxHeader.StdId = OLDStdId;
    } else {
        mes = HAL_CAN_AddTxMessage(easyStruct->hcan, &easyStruct->TxHeader, aData, &easyStruct->pTxMailbox);
    }
    return mes;
}

/* Funzione per ricevere un messaggio CAN */
static inline uint16_t Easycan_GetMessage(Easycan_Struct *easyStruct, uint8_t Data[]) {
    uint32_t canCurrTime = HAL_GetTick();

    // Attende la ricezione del messaggio CAN fino al timeout
    while (HAL_CAN_GetRxMessage(easyStruct->hcan, easyStruct->RxFifo, &easyStruct->RxHeader, Data) != HAL_OK
           && HAL_GetTick() - canCurrTime < easyStruct->canTimeout);

    if (easyStruct->RxHeader.StdId == easyStruct->TxHeader.StdId
        && easyStruct->RxHeader.IDE == CAN_ID_STD
        && easyStruct->RxHeader.RTR == CAN_RTR_DATA
        && HAL_GetTick() - canCurrTime < easyStruct->canTimeout
        && easyStruct->RxHeader.DLC == easyStruct->TxHeader.DLC) {
        return easyStruct->RxHeader.StdId;
    } else {
        return ID_NON_VALIDO;
    }
}

#endif /* EASYCAN_H */


