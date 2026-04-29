#ifndef __CAN_COMMON_H
#define __CAN_COMMON_H

#include "stm32f10x.h"

/* CAN Message IDs */
#define CAN_ID_NUMBERS  0x010000A1
#define CAN_ID_OPERATION 0x010000A2

/* CAN Operation Codes */
#define CAN_OP_ADD  0x01
#define CAN_OP_SUB  0x02
#define CAN_OP_MUL  0x03
#define CAN_OP_DIV  0x04

/* CAN Status Flags */
extern volatile uint8_t CAN1_RxPending;
extern volatile uint8_t CAN2_RxPending;
extern volatile CanRxMsg CAN1_RxMessage;
extern volatile CanRxMsg CAN2_RxMessage;

/* Function Prototypes */
void CAN1_Config(void);
void CAN2_Config(void);
uint8_t CAN1_Send_Message(uint32_t id, uint8_t *data, uint8_t len);
uint8_t CAN2_Send_Message(uint32_t id, uint8_t *data, uint8_t len);

#endif /* __CAN_COMMON_H */
