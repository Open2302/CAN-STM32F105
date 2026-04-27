#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

volatile CanRxMsg CAN1_RxMessage;
volatile uint8_t CAN1_RxPending = 0;
volatile uint8_t CAN1_TxBusy = 0;

/* Filter Config for CAN1 */
void CAN1_Filter_Config(void)
{
    CAN_FilterInitTypeDef CAN_FilterInitStructure;

    CAN_FilterInitStructure.CAN_FilterNumber = 0; /* Filter 0 initialization */
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; /* Mask mode */
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; /* 32 bit */

    // Accept ALL messages
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;

    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0; /* First input buffer */
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
}
void CAN1_Config(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // PA11=RX (Input)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // Input Pull-Up
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PA12=TX (AF_PP)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // Alternate Push pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // CAN Initialization
    CAN_InitTypeDef CAN_InitStructure;
    CAN_DeInit(CAN1);

//    CAN_StructInit(&CAN_InitStructure);

    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; /* Normal mode */
    CAN_InitStructure.CAN_TTCM = DISABLE; /* Working on event not on time */
    CAN_InitStructure.CAN_ABOM = ENABLE; /* Auto bus-off management for troubles */
    CAN_InitStructure.CAN_AWUM = DISABLE;   /* Auto wake-up */
    CAN_InitStructure.CAN_NART = ENABLE;  /* Auto retransmit */
    CAN_InitStructure.CAN_RFLM = DISABLE; /* Rewrite */
    CAN_InitStructure.CAN_TXFP = DISABLE;   /* FIFO priority order */

    /* Timing */
    CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq; /* 1 of time */
    CAN_InitStructure.CAN_BS1  = CAN_BS1_4tq; /* Bit sampling point */
    CAN_InitStructure.CAN_BS2  = CAN_BS2_4tq; /* Bit sampling point */
    CAN_InitStructure.CAN_Prescaler = 32; /* 125 K/bit */

    CAN_Init(CAN1, &CAN_InitStructure);

    CAN1_Filter_Config();

    // RX FIFO0 pending when new message
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

    // NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn; /* FIFO0 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/* CAN1 Send Message */
uint8_t CAN1_Send_Message(uint32_t id, uint8_t *data, uint8_t len)
{
    if (CAN1_TxBusy) return 0;  // The previous transmission is still going

    CanTxMsg TxMessage; /* Convert */
    TxMessage.ExtId = (id >> 3) & 0x1FFFFFFF; // Extended ID
    TxMessage.IDE = CAN_Id_Extended;        // Extended ID
    TxMessage.RTR = CAN_RTR_Data; // Remote Transmission Request data
    TxMessage.DLC = len; // lenght that will be transmitted

    // Copying data
    for (uint8_t i = 0; i < len && i < 8; i++) {
        TxMessage.Data[i] = data[i];
    }
    // send into mailbox
    uint8_t mailbox = CAN_Transmit(CAN1, &TxMessage);
    if (mailbox == CAN_TxStatus_NoMailBox) return 0;

    CAN1_TxBusy = 1;

    while (CAN_TransmitStatus(CAN1, mailbox) == CAN_TxStatus_Pending);

    CAN1_TxBusy = 0;
    return (CAN_TransmitStatus(CAN1, mailbox) == CAN_TxStatus_Ok);
}

void CAN1_RX0_IRQHandler(void)
{
	if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET) {
		CAN_Receive(CAN1, CAN_FIFO0, (CanRxMsg*)&CAN1_RxMessage);
		CAN1_RxPending = 1;
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	}
}
