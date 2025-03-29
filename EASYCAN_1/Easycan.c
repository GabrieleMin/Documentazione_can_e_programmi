#ifndef EASYCAN_H
#define EASYCAN_H

#include "stm32f4xx_hal.h"  // Includi la libreria per il microcontrollore STM32F4

#ifdef HAL_UART_MODULE_ENABLED  // Se il modulo UART è abilitato
#include "stm32f4xx_hal_uart.h"  // Include l'header UART per la gestione della comunicazione seriale
#endif

#include <string.h>  // Per le funzioni di manipolazione delle stringhe
#include <stdlib.h>  // Per funzioni di gestione della memoria e conversioni

// Definizioni di costanti
#define PARAMETRO_NON_NECESSARIO  0xFFFFFFFF  // Parametro per identificare un valore non necessario
#define ID_NON_VALIDO             0xFFFF       // ID che indica un messaggio non valido

// Struttura per la configurazione della comunicazione CAN
typedef struct {
    CAN_TxHeaderTypeDef TxHeader;   // Intestazione del messaggio di trasmissione
    CAN_RxHeaderTypeDef RxHeader;   // Intestazione del messaggio ricevuto
    uint32_t pTxMailbox;            // Cella di trasmissione CAN
    CAN_HandleTypeDef *hcan;        // Gestore della periferica CAN
    uint32_t RxFifo;                // FIFO di ricezione
    uint32_t canTimeout;            // Timeout per la ricezione dei messaggi CAN
} Easycan_Struct;

/**
 * Funzione di inizializzazione della struttura Easycan.
 * Imposta i parametri di base per la trasmissione CAN e associa il canale.
 */
inline void Easycan_init(Easycan_Struct *easyStruct, CAN_HandleTypeDef *hcan, uint32_t RxFifo) {
    easyStruct->hcan = hcan;       // Associa il gestore CAN
    easyStruct->RxFifo = RxFifo;   // Imposta la FIFO di ricezione
    easyStruct->canTimeout = 10;    // Timeout di default per la ricezione dei messaggi

    // Imposta i parametri predefiniti per l'intestazione del messaggio di trasmissione
    easyStruct->TxHeader.DLC = 8;                     // Lunghezza del messaggio (8 byte di default)
    easyStruct->TxHeader.ExtId = 0;                   // ID esteso, non utilizzato in questo caso
    easyStruct->TxHeader.IDE = CAN_ID_STD;            // Modalità di identificatore standard
    easyStruct->TxHeader.RTR = CAN_RTR_DATA;          // Tipo di richiesta (dati)
    easyStruct->TxHeader.StdId = 0;                    // ID standard del messaggio (default a 0)
    easyStruct->TxHeader.TransmitGlobalTime = DISABLE; // Disabilita la trasmissione globale del tempo
}

#ifdef HAL_UART_MODULE_ENABLED
/**
 * Funzione per inviare un messaggio via UART.
 * Utilizza la funzione HAL_UART_Transmit per inviare una stringa.
 */
void serialPrint(UART_HandleTypeDef *huart2, char *str) {
    HAL_UART_Transmit(huart2, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}

/**
 * Funzione per inviare un messaggio via UART con un ritorno a capo.
 */
void serialPrintln(UART_HandleTypeDef *huart2, char *str) {
    serialPrint(huart2, str);  // Invio della stringa
    char newline[] = "\r\n";    // Aggiunge un ritorno a capo
    HAL_UART_Transmit(huart2, (uint8_t *)newline, strlen(newline), HAL_MAX_DELAY);
}
#endif

/**
 * Funzione per inviare un messaggio CAN.
 * Se un ID di risposta è fornito, invia il messaggio con l'ID specificato.
 * Altrimenti, utilizza l'ID di default.
 */
inline HAL_StatusTypeDef Easycan_SendMessage(Easycan_Struct *easyStruct, uint8_t aData[], uint32_t ResponseforthisID) {
    HAL_StatusTypeDef mes;

    // Se è specificato un ID di risposta diverso dal valore non necessario
    if (ResponseforthisID != PARAMETRO_NON_NECESSARIO) {
        uint32_t OLDStdId = easyStruct->TxHeader.StdId;  // Salva l'ID corrente
        easyStruct->TxHeader.StdId = ResponseforthisID;   // Modifica l'ID del messaggio
        mes = HAL_CAN_AddTxMessage(easyStruct->hcan, &easyStruct->TxHeader, aData, &easyStruct->pTxMailbox);  // Invia il messaggio
        easyStruct->TxHeader.StdId = OLDStdId;  // Ripristina l'ID originale
    } else {
        // Se non è specificato un ID di risposta, invia con l'ID di default
        mes = HAL_CAN_AddTxMessage(easyStruct->hcan, &easyStruct->TxHeader, aData, &easyStruct->pTxMailbox);
    }

    return mes;  // Restituisce lo stato dell'operazione di invio
}

/**
 * Funzione per ricevere un messaggio CAN.
 * La funzione blocca l'esecuzione fino a quando un messaggio CAN valido viene ricevuto o il timeout scade.
 * Restituisce l'ID del messaggio ricevuto o un ID non valido se non viene ricevuto alcun messaggio.
 */
inline uint16_t Easycan_GetMessage(Easycan_Struct *easyStruct, uint8_t Data[]) {
    uint32_t canCurrTime = HAL_GetTick();  // Memorizza il tempo attuale

    // Attende la ricezione del messaggio CAN fino al timeout
    while (HAL_CAN_GetRxMessage(easyStruct->hcan, easyStruct->RxFifo, &easyStruct->RxHeader, Data) != HAL_OK
           && HAL_GetTick() - canCurrTime < easyStruct->canTimeout);

    // Verifica che il messaggio ricevuto sia valido
    if (easyStruct->RxHeader.StdId == easyStruct->TxHeader.StdId
        && easyStruct->RxHeader.IDE == CAN_ID_STD
        && easyStruct->RxHeader.RTR == CAN_RTR_DATA
        && HAL_GetTick() - canCurrTime < easyStruct->canTimeout
        && easyStruct->RxHeader.DLC == easyStruct->TxHeader.DLC) {
        return easyStruct->RxHeader.StdId;  // Restituisce l'ID del messaggio ricevuto
    } else {
        return ID_NON_VALIDO;  // Restituisce un ID non valido se il messaggio non è valido
    }
}

#endif /* EASYCAN_H */

