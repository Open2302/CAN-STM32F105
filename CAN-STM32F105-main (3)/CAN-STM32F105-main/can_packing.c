#include "can_packing.h"
#include <string.h>

void Pack_Numbers_Message(int32_t num1, int32_t num2, uint8_t *buffer)
{
    buffer[0] = (num1 >> 0)  & 0xFF;
    buffer[1] = (num1 >> 8)  & 0xFF;
    buffer[2] = (num1 >> 16) & 0xFF;
    buffer[3] = (num1 >> 24) & 0xFF;

    buffer[4] = (num2 >> 0)  & 0xFF;
    buffer[5] = (num2 >> 8)  & 0xFF;
    buffer[6] = (num2 >> 16) & 0xFF;
    buffer[7] = (num2 >> 24) & 0xFF;
}

void Unpack_Numbers_Message(uint8_t *buffer, int32_t *num1, int32_t *num2)
{
	*num1 = ((int32_t)buffer[0] << 0)  |
	        ((int32_t)buffer[1] << 8)  |
	        ((int32_t)buffer[2] << 16) |
	        ((int32_t)buffer[3] << 24);

	*num2 = ((int32_t)buffer[4] << 0)  |
	        ((int32_t)buffer[5] << 8)  |
	        ((int32_t)buffer[6] << 16) |
	        ((int32_t)buffer[7] << 24);
}

uint8_t Command_To_CAN_Op(const char *cmd)
{
    if (strcmp(cmd, "add") == 0) return CAN_OP_ADD;
    if (strcmp(cmd, "sub") == 0) return CAN_OP_SUB;
    if (strcmp(cmd, "mul") == 0) return CAN_OP_MUL;
    if (strcmp(cmd, "div") == 0) return CAN_OP_DIV;
    return 0xFF;  // Unknown
}

/* Convert CAN operation code to string */
const char* CAN_Op_To_String(uint8_t code)
{
    switch (code) {
        case CAN_OP_ADD: return "add";
        case CAN_OP_SUB: return "sub";
        case CAN_OP_MUL: return "mul";
        case CAN_OP_DIV: return "div";
        default: return "unknown";
    }
}
