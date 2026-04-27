#ifndef __CAN_PACKING_H
#define __CAN_PACKING_H

#include "stm32f10x.h"

#define CAN_ID_NUMBERS   0x010000A1
#define CAN_ID_OPERATION 0x010000A2

#define CAN_OP_ADD 0x01
#define CAN_OP_SUB 0x02
#define CAN_OP_MUL 0x03
#define CAN_OP_DIV 0x04

void Pack_Numbers_Message(int32_t n1, int32_t n2, uint8_t *buf);
void Unpack_Numbers_Message(uint8_t *buf, int32_t *n1, int32_t *n2);
uint8_t Command_To_CAN_Op(const char *cmd);
const char* CAN_Op_To_String(uint8_t code);

#endif
