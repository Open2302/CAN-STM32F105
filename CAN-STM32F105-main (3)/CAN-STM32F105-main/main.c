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

extern volatile uint8_t CAN1_RxPending;
extern volatile CanRxMsg CAN1_RxMessage;
extern volatile uint8_t CAN2_RxPending;
extern volatile CanRxMsg CAN_RxMessage;
extern volatile uint32_t CAN1_Debug_Counter;

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
void SystemClock_Config(void)
{
    ErrorStatus HSEStartUpStatus;
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if (HSEStartUpStatus == SUCCESS)
    {
        FLASH_SetLatency(FLASH_Latency_2);
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        RCC_PCLK1Config(RCC_HCLK_Div2);
        RCC_PCLK2Config(RCC_HCLK_Div1);
        RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
        RCC_PLLCmd(ENABLE);
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        while (RCC_GetSYSCLKSource() != 0x08);
    }
    else { while(1); }

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

void CAN_Send_Calculated_Data(void)
{
    uint8_t data[8];
    uint8_t op_byte;

    // CAN DIP3
    uint8_t use_can2 = DIP_Get_CAN_Select();

    // Pack Numbers
    Pack_Numbers_Message(Console_Num1, Console_Num2, data);
    if (use_can2)
    {
		// CAN2_Send_Message(CAN_ID_NUMBERS, data, 8);
    	CAN2_Send_Message(CAN_ID_NUMBERS, data, 8);
    	DMA_SendString("\r\n CAN2");
	} else {
		CAN1_Send_Message(CAN_ID_NUMBERS, data, 8);
		DMA_SendString("\r\n CAN1");

	}
    // Send operation
    op_byte = Console_Op;
    if (use_can2)
    {
    	CAN2_Send_Message(CAN_ID_OPERATION, &op_byte, 1);
	} else {
		CAN1_Send_Message(CAN_ID_OPERATION, &op_byte, 1);
	}
    Console_Num1_Ready = 0;
    Console_Num2_Ready = 0;
    DMA_SendString("\r\n[CAN] Sent ");
    DMA_SendString(use_can2 ? "CAN2" : "CAN1");
    DMA_SendString("\r\n> ");
}

void CAN_Process_Received(void)
{
    // CAN1
    if (CAN1_RxPending) {
        CAN1_RxPending = 0;
        DMA_SendString("\r\n[DBG] CAN1 message received!\r\n");
        if (CAN1_RxMessage.ExtId == CAN_ID_NUMBERS) {
        	DMA_SendString("[DBG] Got NUMBERS message\r\n");
            Unpack_Numbers_Message(CAN1_RxMessage.Data,
                                  &CAN_Received_Num1,
                                  &CAN_Received_Num2);
            CAN_Pending_Numbers = 1;
        }
        else if (CAN1_RxMessage.ExtId == (CAN_ID_OPERATION)) {
        	DMA_SendString("[DBG] Got OPERATION message\r\n");
            CAN_Received_Op = CAN1_RxMessage.Data[0];
            CAN_Pending_Op = 1;
        }
    }

    // CAN2
    if (CAN2_RxPending) {
        CAN2_RxPending = 0;

        if (CAN2_RxMessage.ExtId == CAN_ID_NUMBERS) {
            Unpack_Numbers_Message(CAN2_RxMessage.Data,
                                  &CAN_Received_Num1,
                                  &CAN_Received_Num2);
            CAN_Pending_Numbers = 1;
        }
        else if (CAN2_RxMessage.ExtId == (CAN_ID_OPERATION)) {
            CAN_Received_Op = CAN2_RxMessage.Data[0];
            CAN_Pending_Op = 1;
        }
    }

    if (CAN_Pending_Numbers && CAN_Pending_Op) {
    	DMA_SendString("[DBG] Calculating result...\r\n");
        CAN_Pending_Numbers = 0;
        CAN_Pending_Op = 0;

        int32_t result = 0;
        char buf[12];

        switch (CAN_Received_Op) {
            case CAN_OP_ADD: result = CAN_Received_Num1 + CAN_Received_Num2; break;
            case CAN_OP_SUB: result = CAN_Received_Num1 - CAN_Received_Num2; break;
            case CAN_OP_MUL: result = CAN_Received_Num1 * CAN_Received_Num2; break;
            case CAN_OP_DIV:
                if (CAN_Received_Num2 != 0) {
                    result = CAN_Received_Num1 / CAN_Received_Num2;
                } else {
                    DMA_SendString("\r\n[CAN] Error: div by zero\r\n> ");
                    return;
                }
                break;
            default:
                DMA_SendString("\r\n[CAN] Error: unknown op\r\n> ");
                return;
        }

        DMA_SendString("\r\n[CAN] Result: ");
        DMA_SendNumber(CAN_Received_Num1);
        DMA_SendString(" ");
        DMA_SendString(CAN_Op_To_String(CAN_Received_Op));
        DMA_SendString(" ");
        DMA_SendNumber(CAN_Received_Num2);
        DMA_SendString(" = ");
        DMA_SendNumber(result);
        DMA_SendString("\r\n> ");
    }
}

/* USART and DMA Config */
void USART_DMA_Config(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* GPIO: PA9=TX, PA10=RX */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART init */
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate            = 19200;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    /* Enable DMA requests */
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    USART_Cmd(USART1, ENABLE);

    /* DMA RX */
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)DMA_RxBuffer;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = 64;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    /* Interrupt on Transfer Complete */
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);

    DMA_Cmd(DMA1_Channel5, ENABLE);

    /* DMA TX */
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)DMA_TxBuffer;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = 0; /* Set on transfer */
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE); // Transfer end

    /* NVIC for RX */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* NVIC for TX */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_Init(&NVIC_InitStructure);
}

/* Old USART Send String but on DMA */
void DMA_SendString(const char *str)
{
    while (TxInProgress == 1);

    uint16_t len = 0;
    while (*str && len < 63) { /* Current Symbol */
        DMA_TxBuffer[len++] = *str++; /* Write in buffer */
    }

    DMA_SetCurrDataCounter(DMA1_Channel4, len); /* Write Len in register */
    TxInProgress = 1;
    DMA_Cmd(DMA1_Channel4, ENABLE);
}

void DMA_SendNumber(int32_t num)
{
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
void USART_SendChar(USART_TypeDef* USARTx, char c)
{
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    USART_SendData(USARTx, c);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}

/* Checking and processing reception via DMA */
void DMA_CheckRX(void)
{
    static uint8_t local_RxBuffer[64];
    static uint8_t local_RxIndex = 0;

    //Getting the current position of the DMA
    uint16_t DMA_CurrentPos = 64 - DMA_GetCurrDataCounter(DMA1_Channel5);

    //Processing all new bytes
    while (DMA_RxLastPos != DMA_CurrentPos)
    {
        uint8_t data = DMA_RxBuffer[DMA_RxLastPos];
        USART_SendChar(USART1, data);

        if (data == '\b' || data == 127) {
            if (local_RxIndex > 0) {
                local_RxIndex--;
                USART_SendChar(USART1, ' ');
                USART_SendChar(USART1, '\b');
            }
        }
        else if (data == '\r' || data == '\n') {

            memcpy((void*)RxBuffer, local_RxBuffer, local_RxIndex);
            RxBuffer[local_RxIndex] = '\0';
            RxReady = 1;

            memset(local_RxBuffer, 0, 64);
            local_RxIndex = 0;

            DMA_RxLastPos = (DMA_RxLastPos + 1) % 64;
            return;
        }
        else {
            if (local_RxIndex < 63) {
                local_RxBuffer[local_RxIndex++] = data;
            }
        }
        // Next pos
        DMA_RxLastPos = (DMA_RxLastPos + 1) % 64;
    }
}

// Interrupt handler for TX
void DMA1_Channel4_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC4) != RESET) { //Tx complete
        TxInProgress = 0; // Reset flag
        DMA_Cmd(DMA1_Channel4, DISABLE);
        DMA_ClearITPendingBit(DMA1_IT_TC4);
    }
}

void LED_GPIO_Config(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void TIM2_Config(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseStructure.TIM_Period    = 1000 - 1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_EnableIRQ(TIM2_IRQn);
    TIM_Cmd(TIM2, ENABLE);
}

void DIP_Config()
{
	/* Enable GPIOC clocking */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

	//Structure name for EXTI
	GPIO_InitTypeDef  GPIO_InitStructure;
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

uint8_t DIP_Get_CAN_Select(void)
{
    return !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4);
}

/* Send trigger flag */
volatile uint8_t CAN_Send_Trigger = 0;

/* The interrupt handler for EXTI */
void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        CAN_Send_Trigger = 1;
        EXTI1_Interrupt = 1;
        lastTime = millis();
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update)) /* Flag check */
    {
        uwTick++; /* Tick +1 if flag is different */
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); /* Clear Flag */

        if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_15) == Bit_RESET)
            GPIO_SetBits(GPIOB, GPIO_Pin_15);
        else
            GPIO_ResetBits(GPIOB, GPIO_Pin_15);
    }
}

void Command_Execute(char *cmd, int32_t val1, int32_t val2)
{
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
    }
    else if (strcmp(cmd, "sub") == 0) {
        result = val1 - val2;
        DMA_SendString("\r\nsub: ");
        DMA_SendNumber(val1);
        DMA_SendString(" - ");
        DMA_SendNumber(val2);
        DMA_SendString(" = ");
        DMA_SendNumber(result);
        DMA_SendString("\r\n> ");
    }
    else if (strcmp(cmd, "mul") == 0) {
        result = val1 * val2;
        DMA_SendString("\r\nmul: ");
        DMA_SendNumber(val1);
        DMA_SendString(" * ");
        DMA_SendNumber(val2);
        DMA_SendString(" = ");
        DMA_SendNumber(result);
        DMA_SendString("\r\n> ");
    }
    else if (strcmp(cmd, "div") == 0) {
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
    }
    else {
        DMA_SendString("\r\nerror: unknown command\r\n> ");
    }
}

void Command_Parse(uint8_t *buffer) /* String to int and cmd */
{
    char cmd[16] = {0}; /* array for commands */
    int32_t val1 = 0, val2 = 0;
    uint8_t op_code = 0;

    /* Find first space: separates cmd from num1 */
    char *space1 = strchr((char*)buffer, ' ');
    if (space1 == NULL) {
        /* No arguments - invalid command */
        DMA_SendString("\r\nerror: expected two numbers\r\n> ");
        return;
    }
    *space1 = '\0'; /* Terminate command string */
    strncpy(cmd, (char*)buffer, 15);
    cmd[15] = '\0';

    /* Find second space: separates num1 from num2 */
    char *space2 = strchr(space1 + 1, ' ');
    if (space2 == NULL) {
        /* Only one number - invalid */
        DMA_SendString("\r\nerror: expected two numbers\r\n> ");
        return;
    }
    *space2 = '\0'; /* Terminate num1 string */

    /* Parse numbers */
    char *num1_ptr = space1 + 1;
    char *num2_ptr = space2 + 1;

    /* Skip leading spaces */
    while (*num1_ptr == ' ') num1_ptr++;
    while (*num2_ptr == ' ') num2_ptr++;

    val1 = atoi(num1_ptr); /* Converting first string to a number */
    val2 = atoi(num2_ptr); /* Converting second string to a number */

    op_code = Command_To_CAN_Op(cmd);
		if (op_code == 0xFF) {
		    DMA_SendString("\r\nerror: unknown command (use add/sub/mul/div)\r\n> ");
		    Console_Num1_Ready = 0;
		    Console_Num2_Ready = 0;
		    return;
		}
    Console_Num1 = val1;
	Console_Num2 = val2;
	Console_Op = op_code;
	Console_Num1_Ready = 1;
	Console_Num2_Ready = 1;

    /* Pass the parsed command to the handler */
    Command_Execute(cmd, val1, val2);
}

void USART1_Calculation(void)
{
    if (RxReady == 1) {
        RxReady = 0;
        Command_Parse(RxBuffer);
        memset((void*)RxBuffer, 0, sizeof(RxBuffer));
    }
}
void EXTI1_Name_Output(void)
{
	if (EXTI1_Interrupt == 1)
	{
//	    		EXTI1_Interrupt = 0;
		if ((millis() - lastTime) >= 5)
		{
			EXTI1_Interrupt = 0;
			/* Checking Current Status of EXTI1 */
			if ((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == 1) /*&& (State == 0)*/)
			{
//				DMA_SendString("Simonov Egor\r\n> ");
			}
//					State = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5);
		}
//	    	    	lastTime = millis();
	}
}


void Delay_ms(volatile uint32_t ms)
{
    uint32_t start = uwTick;
    while ((uwTick - start) < ms);
}

uint32_t millis(void)
{
    return uwTick; /* Returning uwTick value */
}


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemClock_Config(); /* Configure system clock: HSE -> 72 MHz */
    TIM2_Config();        /* Configure TIM2 for 1ms ticks */
    lastTime = millis();  /* Record startup time */
    USART_DMA_Config();   /* Configure USART1 with DMA1 for TX */
    LED_GPIO_Config();    /* Configure LED on PB15 */
    DIP_Config();       /* Configure EXTI for DIP switch on PC5 */
    CAN1_Config();
    CAN2_Config();
    __enable_irq();

    /* Resetting variables */
    memset((void*)RxBuffer, 0, sizeof(RxBuffer));
    RxIndex = 0;
    RxReady = 0;
    TxInProgress = 0;

    DMA_SendString("Commands: add, sub, mul, div\r\n");
    DMA_SendString("> ");
    while (1)
    {
	 if (RxReady == 1) {
				RxReady = 0;
				Command_Parse(RxBuffer);
				memset((void*)RxBuffer, 0, sizeof(RxBuffer));
			}
		if (CAN_Send_Trigger == 1) {
			CAN_Send_Trigger = 0;
			if (Console_Num1_Ready && Console_Num2_Ready && Console_Op != 0xFF) {
				CAN_Send_Calculated_Data();
			} else {
				DMA_SendString("\r\nError");
			}
		}
		static uint32_t last_cnt = 0;
		if (CAN1_Debug_Counter != last_cnt) {
		    last_cnt = CAN1_Debug_Counter;
		    DMA_SendString("IRQ CNT: ");  // Hard_Print - вывод мину€ DMA
		    char buf[16];
		    sprintf(buf, "%lu\r\n", last_cnt);
		    DMA_SendString(buf);
		}

    	DMA_CheckRX();
    	CAN_Process_Received();

        /* Process DIP switch */
        EXTI1_Name_Output();

        /* Process USART commands */
        USART1_Calculation();
    }

}





