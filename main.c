#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_dma.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f10x_can.h"
#include "can_common.h"
#include "can_packing.h"

/* DMA Buffers */
uint8_t DMA_RxBuffer[64];
uint8_t DMA_TxBuffer[64];

// Flags Transmit
volatile uint8_t Remote_Numbers_Ready = 0;
volatile uint8_t Remote_Op_Ready = 0;

// Data
volatile int32_t Remote_Num1 = 0;
volatile int32_t Remote_Num2 = 0;
volatile uint8_t Remote_Op = 0;

// Flags receive
volatile uint8_t CAN_Numbers_Received = 0;
volatile uint8_t CAN_Op_Received = 0;
volatile int32_t CAN_Recv_Num1 = 0;
volatile int32_t CAN_Recv_Num2 = 0;
volatile uint8_t CAN_Recv_Op = 0;

// Protect
volatile uint8_t Last_Sent_Type = 0; // 0=nothing, 1=numbers, 2=command

extern volatile uint8_t CAN1_RxPending;
extern volatile CanRxMsg CAN1_RxMessage;
extern volatile uint8_t CAN2_RxPending;
extern volatile CanRxMsg CAN2_RxMessage;

/* Cmd buffer */
volatile uint8_t RxBuffer[64];
volatile uint8_t RxIndex = 0;
volatile uint8_t RxReady = 0;

volatile uint8_t TxInProgress = 0;

volatile uint16_t DMA_RxLastPos = 0;

volatile uint32_t uwTick = 0;
volatile uint32_t lastTime = 0;
uint8_t EXTI1_Interrupt = 0;

/* Functions Prototypes */
void SystemClock_Config(void);
void CAN_Send_Calculated_Data(void);
void CAN_Process_Received(void);
void USART_DMA_Config(void);
void LED_GPIO_Config(void);
void TIM2_Config(void);
void DIP_Config(void);
uint8_t DIP_Get_CAN_Select(void);
void DMA_SendNumber(int32_t num);
void DMA_SendString(const char *str);
void DMA_CheckRX(void);
void Command_Execute(char *cmd, int32_t val1, int32_t val2);
void Command_Parse(uint8_t *buffer);
void USART1_Calculation(void);
void DMA1_Channel4_IRQHandler(void);
void DMA1_Channel5_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI1_Name_Output(void);
void USART_SendChar(USART_TypeDef* USARTx, char c);
uint32_t millis(void);
void Delay_ms(volatile uint32_t ms);

/* System Clocking 72MHz */
void SystemClock_Config(void) {
	ErrorStatus HSEStartUpStatus;
	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if (HSEStartUpStatus == SUCCESS) {
		FLASH_SetLatency(FLASH_Latency_2);
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
		RCC_PLLCmd(ENABLE);
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
			;
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		while (RCC_GetSYSCLKSource() != 0x08)
			;
	} else {
		while (1)
			;
	}

	SystemCoreClock = 72000000UL;
}

/* Can */
volatile uint8_t CAN_Pending_Numbers = 0;
volatile uint8_t CAN_Pending_Op = 0;
volatile int32_t CAN_Received_Num1 = 0;
volatile int32_t CAN_Received_Num2 = 0;
volatile uint8_t CAN_Received_Op = 0;

/* Console Variables (from USART) */
volatile uint8_t Console_Num1_Ready = 0;
volatile uint8_t Console_Num2_Ready = 0;
volatile int32_t Console_Num1 = 0;
volatile int32_t Console_Num2 = 0;
volatile uint8_t Console_Op = 0;

void CAN_Send_Trigger_Handler(void) {
	uint8_t use_can2 = DIP_Get_CAN_Select();
	uint8_t sent_something = 0;

	if (Remote_Numbers_Ready && Last_Sent_Type != 1) {
		uint8_t data[8];
		Pack_Numbers_Message(Remote_Num1, Remote_Num2, data);

		uint8_t result;
		if (use_can2) {
			result = CAN2_Send_Message(CAN_ID_NUMBERS, data, 8);
		} else {
			result = CAN1_Send_Message(CAN_ID_NUMBERS, data, 8);
		}

		if (result) {
			DMA_SendString("\r\n Numbers sent ");
			DMA_SendString(use_can2 ? "CAN2" : "CAN1");
			DMA_SendString("\r\n> ");
			Remote_Numbers_Ready = 0;
			Last_Sent_Type = 1;
			sent_something = 1;
		} else {
			DMA_SendString("\r\n[ERROR] \r\n> ");
		}
	} else if (Remote_Numbers_Ready && Last_Sent_Type == 1) {
		DMA_SendString(
				"\r\n Numbers already sent, send command \r\n> ");
	}

	if (Remote_Op_Ready && Last_Sent_Type != 2) {
		uint8_t op_byte = Remote_Op;

		uint8_t result;
		if (use_can2) {
			result = CAN2_Send_Message(CAN_ID_OPERATION, &op_byte, 1);
		} else {
			result = CAN1_Send_Message(CAN_ID_OPERATION, &op_byte, 1);
		}

		if (result) {
			DMA_SendString("\r\n[CAN] Command sent ");
			DMA_SendString(use_can2 ? "CAN2" : "CAN1");
			DMA_SendString("\r\n> ");
			Remote_Op_Ready = 0;
			Last_Sent_Type = 2;
			sent_something = 1;
		} else {
			DMA_SendString("\r\n[ERROR] \r\n> ");
		}
	} else if (Remote_Op_Ready && Last_Sent_Type == 2) {
		DMA_SendString(
				"\r\n Command already sent, send numbers\r\n> ");
	}

	if (!sent_something && !Remote_Numbers_Ready && !Remote_Op_Ready) {
		DMA_SendString(
				"\r\n Nothing to send. Enter 'N1 N2' or 'cmd' first\r\n> ");
	}
}

void CAN_Process_Received(void) {
	if (CAN1_RxPending) {
		CAN1_RxPending = 0;

		if (CAN1_RxMessage.ExtId == CAN_ID_NUMBERS) {
			Unpack_Numbers_Message(CAN1_RxMessage.Data, &CAN_Recv_Num1,
					&CAN_Recv_Num2);
			CAN_Numbers_Received = 1;
			DMA_SendString("\r\n Numbers received\r\n> ");
		} else if (CAN1_RxMessage.ExtId == CAN_ID_OPERATION) {
			CAN_Recv_Op = CAN1_RxMessage.Data[0];
			CAN_Op_Received = 1;
			DMA_SendString("\r\n Command received\r\n> ");
		}
	}

	if (CAN2_RxPending) {
		CAN2_RxPending = 0;

		if (CAN2_RxMessage.ExtId == CAN_ID_NUMBERS) {
			Unpack_Numbers_Message(CAN2_RxMessage.Data, &CAN_Recv_Num1,
					&CAN_Recv_Num2);
			CAN_Numbers_Received = 1;
			DMA_SendString("\r\n Numbers received\r\n> ");
		} else if (CAN2_RxMessage.ExtId == CAN_ID_OPERATION) {
			CAN_Recv_Op = CAN2_RxMessage.Data[0];
			CAN_Op_Received = 1;
			DMA_SendString("\r\n Command received\r\n> ");
		}
	}

	if (CAN_Numbers_Received && CAN_Op_Received) {
		int32_t result = 0;

		switch (CAN_Recv_Op) {
		case CAN_OP_ADD:
			result = CAN_Recv_Num1 + CAN_Recv_Num2;
			break;
		case CAN_OP_SUB:
			result = CAN_Recv_Num1 - CAN_Recv_Num2;
			break;
		case CAN_OP_MUL:
			result = CAN_Recv_Num1 * CAN_Recv_Num2;
			break;
		case CAN_OP_DIV:
			if (CAN_Recv_Num2 != 0) {
				result = CAN_Recv_Num1 / CAN_Recv_Num2;
			} else {
				DMA_SendString("\r\n Error: div by zero\r\n> ");
				CAN_Numbers_Received = 0;
				CAN_Op_Received = 0;
				return;
			}
			break;
		default:
			DMA_SendString("\r\n Error: unknown op\r\n> ");
			CAN_Numbers_Received = 0;
			CAN_Op_Received = 0;
			return;
		}

		// Вывод результата
		DMA_SendString("\r\n Result: ");
		DMA_SendNumber(CAN_Recv_Num1);
		DMA_SendString(" ");
		DMA_SendString(CAN_Op_To_String(CAN_Recv_Op));
		DMA_SendString(" ");
		DMA_SendNumber(CAN_Recv_Num2);
		DMA_SendString(" = ");
		DMA_SendNumber(result);
		DMA_SendString("\r\n> ");

		// Сброс для следующего вычисления
		CAN_Numbers_Received = 0;
		CAN_Op_Received = 0;
	}
}

/* USART and DMA Config */
void USART_DMA_Config(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA,
			ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* GPIO: PA9=TX, PA10=RX */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART init */
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	/* Enable DMA requests */
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	USART_Cmd(USART1, ENABLE);

	/* DMA RX */
	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA1_Channel5);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &USART1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) DMA_RxBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 64;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);

	/* Interrupt on Transfer Complete */
	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);

	DMA_Cmd(DMA1_Channel5, ENABLE);

	/* DMA TX */
	DMA_DeInit(DMA1_Channel4);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &USART1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) DMA_TxBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = 0; /* Set on transfer */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);

	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE); // Transfer end

	/* NVIC for RX */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* NVIC for TX */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

/* Old USART Send String but on DMA */
void DMA_SendString(const char *str) {
	uint32_t timeout = 100000;
	while (TxInProgress == 1 && timeout--)
		;
	if (TxInProgress == 1) {
		DMA_Cmd(DMA1_Channel4, DISABLE);
		TxInProgress = 0;
	}

	uint16_t len = 0;
	while (*str && len < 63) { /* Current Symbol */
		DMA_TxBuffer[len++] = *str++; /* Write in buffer */
	}

	DMA_SetCurrDataCounter(DMA1_Channel4, len); /* Write Len in register */
	TxInProgress = 1;
	DMA_Cmd(DMA1_Channel4, ENABLE);
}

void DMA_SendNumber(int32_t num) {
	char buf[32];
	int len = 0;

	if (num < 0) {
		buf[len++] = '-';
		num = -num;
	}

	int start = len;
	if (num == 0) {
		buf[len++] = '0';
	} else {
		while (num > 0 && len < 30) {
			buf[len++] = (num % 10) + '0';
			num /= 10;
		}
	}

	for (int i = start, j = len - 1; i < j; i++, j--) {
		char tmp = buf[i];
		buf[i] = buf[j];
		buf[j] = tmp;
	}

	buf[len] = '\0';
	DMA_SendString(buf);
}
/* Echo */
void USART_SendChar(USART_TypeDef* USARTx, char c) {
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
		;
	USART_SendData(USARTx, c);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
		;
}

/* Checking and processing reception via DMA */
void DMA_CheckRX(void) {
	static uint8_t local_RxBuffer[64];
	static uint8_t local_RxIndex = 0;

	//Getting the current position of the DMA
	uint16_t DMA_CurrentPos = 64 - DMA_GetCurrDataCounter(DMA1_Channel5);

	//Processing all new bytes
	while (DMA_RxLastPos != DMA_CurrentPos) {
		uint8_t data = DMA_RxBuffer[DMA_RxLastPos];
		USART_SendChar(USART1, data);

		if (data == '\b' || data == 127) {
			if (local_RxIndex > 0) {
				local_RxIndex--;
				USART_SendChar(USART1, ' ');
				USART_SendChar(USART1, '\b');
			}
		} else if (data == '\r' || data == '\n') {

			memcpy((void*) RxBuffer, local_RxBuffer, local_RxIndex);
			RxBuffer[local_RxIndex] = '\0';
			RxReady = 1;

			memset(local_RxBuffer, 0, 64);
			local_RxIndex = 0;

			DMA_RxLastPos = (DMA_RxLastPos + 1) % 64;
			return;
		} else {
			if (local_RxIndex < 63) {
				local_RxBuffer[local_RxIndex++] = data;
			}
		}
		// Next pos
		DMA_RxLastPos = (DMA_RxLastPos + 1) % 64;
	}
}

// Interrupt handler for TX
void DMA1_Channel4_IRQHandler(void) {
	if (DMA_GetITStatus(DMA1_IT_TC4) != RESET) { //Tx complete
		TxInProgress = 0; // Reset flag
		DMA_Cmd(DMA1_Channel4, DISABLE);
		DMA_ClearITPendingBit(DMA1_IT_TC4);
	}
}

void LED_GPIO_Config(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void TIM2_Config(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };
	TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure = { 0 };
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_EnableIRQ(TIM2_IRQn);
	TIM_Cmd(TIM2, ENABLE);
}

void DIP_Config() {
	/* Enable GPIOC clocking */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

	//Structure name for EXTI
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; /* EXTI1 Pin */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; /* INPUT PULL DOWN like button */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; /* EXTI0 Pin */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; /* INPUT PULL DOWN like button */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5); /* Connection to EXTI */

	/* Structure name for EXTI External interruptions*/
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line5; /* Dip switch on line 5 */
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; /* Trigger on OFF */
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; /* Enable EXTI */
	EXTI_Init(&EXTI_InitStructure);

	/* Structure name for NVIC Interrupt module */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; /* External Line[9:5] Interrupts */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

uint8_t DIP_Get_CAN_Select(void) {
	return !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4);
}

/* Send trigger flag */
volatile uint8_t CAN_Send_Trigger = 0;

/* The interrupt handler for EXTI */
void EXTI9_5_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line5) != RESET) {
		CAN_Send_Trigger = 1;
		EXTI1_Interrupt = 1;
		lastTime = millis();
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
}

void TIM2_IRQHandler(void) {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)) /* Flag check */
	{
		uwTick++; /* Tick +1 if flag is different */
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); /* Clear Flag */

		if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_10) == Bit_RESET)
			GPIO_SetBits(GPIOB, GPIO_Pin_10);
		else
			GPIO_ResetBits(GPIOB, GPIO_Pin_10);
	}
}

void Command_Execute(char *cmd, int32_t val1, int32_t val2) {
	int32_t result = 0;

	if (strcmp(cmd, "add") == 0) {
		result = val1 + val2;
		DMA_SendString("\r\nadd: ");
		DMA_SendNumber(val1);
		DMA_SendString(" + ");
		DMA_SendNumber(val2);
		DMA_SendString(" = ");
		DMA_SendNumber(result);
		DMA_SendString("\r\n> ");
	} else if (strcmp(cmd, "sub") == 0) {
		result = val1 - val2;
		DMA_SendString("\r\nsub: ");
		DMA_SendNumber(val1);
		DMA_SendString(" - ");
		DMA_SendNumber(val2);
		DMA_SendString(" = ");
		DMA_SendNumber(result);
		DMA_SendString("\r\n> ");
	} else if (strcmp(cmd, "mul") == 0) {
		result = val1 * val2;
		DMA_SendString("\r\nmul: ");
		DMA_SendNumber(val1);
		DMA_SendString(" * ");
		DMA_SendNumber(val2);
		DMA_SendString(" = ");
		DMA_SendNumber(result);
		DMA_SendString("\r\n> ");
	} else if (strcmp(cmd, "div") == 0) {
		if (val2 == 0) {
			DMA_SendString("\r\nerror: div by zero\r\n> ");
		} else {
			result = val1 / val2;
			DMA_SendString("\r\ndiv: ");
			DMA_SendNumber(val1);
			DMA_SendString(" / ");
			DMA_SendNumber(val2);
			DMA_SendString(" = ");
			DMA_SendNumber(result);
			DMA_SendString("\r\n> ");
		}
	} else {
		DMA_SendString("\r\nerror: unknown command\r\n> ");
	}
}

void Command_Parse(uint8_t *buffer) {
	char cmd[16] = { 0 };
	int32_t val1 = 0, val2 = 0;
	uint8_t op_code = 0;
	char *space1, *space2;
	uint8_t i = 0;

	while (buffer[i] != '\0') {
		if (buffer[i] == '\r' || buffer[i] == '\n') {
			buffer[i] = '\0';
			break;
		}
		i++;
	}

	space1 = strchr((char*) buffer, ' ');

	if (space1 == NULL) {
		strncpy(cmd, (char*) buffer, 15);
		cmd[15] = '\0';
		op_code = Command_To_CAN_Op(cmd);

		if (op_code != 0xFF) {
			DMA_SendString("\r\n[STORE] Command: ");
			DMA_SendString(cmd);
			DMA_SendString("\r\n[INFO] Toggle DIP4 to send via CAN\r\n> ");
			Remote_Op = op_code;
			Remote_Op_Ready = 1;
		} else {
			DMA_SendString("\r\nerror: unknown command\r\n> ");
		}
		return;
	}

	space2 = strchr(space1 + 1, ' ');

	if (space2 == NULL) {
		*space1 = '\0';
		char *num1_str = (char*) buffer;
		char *num2_str = space1 + 1;

		while (*num1_str == ' ')
			num1_str++;
		while (*num2_str == ' ')
			num2_str++;

		if (strlen(num1_str) > 0 && strlen(num2_str) > 0) {
			val1 = atoi(num1_str);
			val2 = atoi(num2_str);

			Remote_Num1 = val1;
			Remote_Num2 = val2;
			Remote_Numbers_Ready = 1;

			DMA_SendString("\r\n Numbers: ");
			DMA_SendNumber(val1);
			DMA_SendString(" ");
			DMA_SendNumber(val2);
			DMA_SendString("\r\n Toggle DIP4 to send \r\n> ");
		} else {
			DMA_SendString("\r\n error: unknown format\r\n> ");
		}
		return;
	}
	// Local
	*space1 = '\0';
	strncpy(cmd, (char*) buffer, 15);
	cmd[15] = '\0';

	char *num1_str = space1 + 1;
	char *num2_str = space2 + 1;

	while (*num1_str == ' ')
		num1_str++;
	while (*num2_str == ' ')
		num2_str++;

	val1 = atoi(num1_str);
	val2 = atoi(num2_str);
	op_code = Command_To_CAN_Op(cmd);

	if (op_code != 0xFF) {
		Command_Execute(cmd, val1, val2);
	} else {
		DMA_SendString("\r\nerror: unknown command\r\n> ");
	}
}

void USART1_Calculation(void) {
	if (RxReady == 1) {
		RxReady = 0;
		Command_Parse(RxBuffer);
		memset((void*) RxBuffer, 0, sizeof(RxBuffer));
	}
}
void EXTI1_Name_Output(void) {
	if (EXTI1_Interrupt == 1) {
//	    		EXTI1_Interrupt = 0;
		if ((millis() - lastTime) >= 5) {
			EXTI1_Interrupt = 0;
			/* Checking Current Status of EXTI1 */
			if ((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == 1) /*&& (State == 0)*/) {
//				DMA_SendString("Simonov Egor\r\n> ");
			}
//					State = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5);
		}
//	    	    	lastTime = millis();
	}
}
void CAN_Output(void) {
	if (CAN_Send_Trigger == 1) {
		if ((millis() - lastTime) >= 5)
		{
		CAN_Send_Trigger = 0;
		if ((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == 1))
		{
			CAN_Send_Trigger_Handler();
		}
		lastTime = millis();
		}
	}
}

void Delay_ms(volatile uint32_t ms) {
	uint32_t start = uwTick;
	while ((uwTick - start) < ms)
		;
}

uint32_t millis(void) {
	return uwTick; /* Returning uwTick value */
}

int main(void) {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SystemClock_Config();
	TIM2_Config();
	lastTime = millis();
	USART_DMA_Config();
	LED_GPIO_Config();
	DIP_Config();
	CAN1_Config();
	CAN2_Config();
	__enable_irq();

	memset((void*) RxBuffer, 0, sizeof(RxBuffer));
	RxIndex = 0;
	RxReady = 0;
	TxInProgress = 0;

	Remote_Numbers_Ready = 0;
	Remote_Op_Ready = 0;
	CAN_Numbers_Received = 0;
	CAN_Op_Received = 0;
	Last_Sent_Type = 0;

	DMA_SendString("\r\n");
	DMA_SendString("Local: 'add 10 10' [Enter]\r\n");
	DMA_SendString("Can: '10 10' [Enter]  DIP4  'add' [Enter]  DIP4\r\n");
	DMA_SendString("> ");

	while (1) {
		if (RxReady == 1) {
			RxReady = 0;
			Command_Parse(RxBuffer);
			memset((void*) RxBuffer, 0, sizeof(RxBuffer));
		}
		CAN_Output();

		DMA_CheckRX();

		CAN_Process_Received();

		//EXTI1_Name_Output();
	}
}

