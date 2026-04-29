#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

/* CAN2 Buffers */
volatile CanRxMsg CAN2_RxMessage;
volatile uint8_t CAN2_RxPending = 0;
volatile uint8_t CAN2_TxBusy = 0;

/* CAN2 Filter Configuration */
void CAN2_Filter_Config(void)
{
    CAN_FilterInitTypeDef CAN_FilterInitStructure;

    // Filter banks 14-27 are for CAN2 (на STM32F10x с 2 CAN)
    CAN_FilterInitStructure.CAN_FilterNumber = 14;  // First bank for CAN2
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;

    // Accept ALL messages
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;

    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
}

/* CAN2 Initialization - REQUIRES CAN1 INITIALIZED FIRST! */
void CAN2_Config(void)
{
    //  CAN1 MUST be initialized before CAN2!
    // Если CAN1 не инициализирован - CAN2 не заработает!

    // Тактирование
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    // GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // CAN Init (аналогично CAN1)
    CAN_InitTypeDef CAN_InitStructure;
    CAN_DeInit(CAN2);
    CAN_StructInit(&CAN_InitStructure);

    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; /* Normal mode */
	CAN_InitStructure.CAN_TTCM = DISABLE; /* Working on event not on time */
	CAN_InitStructure.CAN_ABOM = ENABLE; /* Auto bus-off management for troubles */
	CAN_InitStructure.CAN_AWUM = DISABLE;   /* Auto wake-up */
	CAN_InitStructure.CAN_NART = ENABLE;  /* Auto retransmit */
	CAN_InitStructure.CAN_RFLM = DISABLE; /* Rewrite */
	CAN_InitStructure.CAN_TXFP = DISABLE;   /* FIFO priority order */

    // Тайминг: 500 кбит/с @ 72 МГц
    CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1  = CAN_BS1_4tq;
    CAN_InitStructure.CAN_BS2  = CAN_BS2_4tq;
    CAN_InitStructure.CAN_Prescaler = 32;

    CAN_Init(CAN2, &CAN_InitStructure);

    // Фильтры
    CAN2_Filter_Config();

    // Прерывания
    CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);

    // NVIC для CAN2 RX
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = CAN2_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/* CAN2 Send Message */
uint8_t CAN2_Send_Message(uint32_t id, uint8_t *data, uint8_t len)
{
    if (CAN2_TxBusy) return 0;

    CanTxMsg TxMessage;
    TxMessage.StdId = (id & 0x7FF);
    TxMessage.ExtId = id & 0x1FFFFFFF;
    TxMessage.IDE   = CAN_Id_Extended;
    TxMessage.RTR   = CAN_RTR_Data;
    TxMessage.DLC   = len;

    for (uint8_t i = 0; i < len && i < 8; i++) {
        TxMessage.Data[i] = data[i];
    }

    uint8_t mailbox = CAN_Transmit(CAN2, &TxMessage);
    if (mailbox == CAN_TxStatus_NoMailBox) return 0;

    CAN2_TxBusy = 1;
    while (CAN_TransmitStatus(CAN2, mailbox) == CAN_TxStatus_Pending);
    CAN2_TxBusy = 0;

    return (CAN_TransmitStatus(CAN2, mailbox) == CAN_TxStatus_Ok);
}

/* CAN2 RX Interrupt Handler */
void CAN2_RX0_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN2, CAN_IT_FMP0) != RESET) {
        CAN_Receive(CAN2, CAN_FIFO0, (CanRxMsg*)&CAN2_RxMessage);
        CAN2_RxPending = 1;
    }
}
