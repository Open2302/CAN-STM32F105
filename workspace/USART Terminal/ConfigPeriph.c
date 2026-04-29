#include "ConfigPeriph.h"

/* System Ñlocking */
void SystemClock_Config()
{
      RCC_DeInit ();                    /* RCC system reset*/

      /* Wait till HSI is ready                                               */
      while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

      RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI); /* Enable HSI */
      while (RCC_GetSYSCLKSource() != 00);

      RCC_HCLKConfig   (RCC_SYSCLK_Div1);   /* HCLK                 		  */
      RCC_PCLK1Config  (RCC_HCLK_Div1);     /* PCLK1  */
      RCC_PCLK2Config  (RCC_HCLK_Div1);     /* PCLK2                  		  */
}

/* USART Configuration */
void USART_Config()
{
	/* Clocking */
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	/* ---GPIO Configuration--- */

	//Structure name for USART
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Configure USART1 Rx - Receive*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10; /* Rx-USART1 PA10 */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING; /* FLOATING to Receive */
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Tx - Transmit */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9; /* Tx-USART1 PA10 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /* Speed for Tx */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP; /* AF_PP Alternate Function Push-Pull specifically for the module USART */
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ---USART Configuration--- */

	//Structure name for USART
	USART_InitTypeDef USART_InitStructure;

	/* USART Configuration
	 *
	 */
	USART_InitStructure.USART_BaudRate = 19200; /* BaudRate = 19200 Bit per Second Speed */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; /* Wordlength(Message) = 8 Bits how many bits will be transmitted between the start and stop bits*/
	USART_InitStructure.USART_StopBits = USART_StopBits_1; /* StopBits = 1 Bit signals the receiver that the data packet has ended*/
	USART_InitStructure.USART_Parity = USART_Parity_No ; /* The Parity bit is not added */
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; /* No Flow Control and no other inputs except Tx, Rx, GND */
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; /* Full Duplex: reception and transmission */
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE); /* Enable USART */
}
void LED_GPIO_Config()
{
	/* Enable GPIOB clocking */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	//Structure name for LED
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; /* Red LED Pin */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* Checking if the STM32 is working */
	GPIO_ResetBits(GPIOB, GPIO_Pin_15); /* Turn on LED */
}
void EXTI1_Config()
{
	/* Enable GPIOC clocking */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

	//Structure name for EXTI
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; /* EXTI1 Pin */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; /* INPUT PULL UP like button */
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM2_Config()
{
	/* Clocking TIM2 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* Structure name for TIM2 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};

	TIM_TimeBaseStructure.TIM_Prescaler = 800 - 1; /* System clocking on HSI */
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1; /* Time Needed P.S 1millis */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; /* The score is up */
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); /* Enable flag update */

	NVIC_InitTypeDef NVIC_InitStructure = {0};
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; /* Interrupt for TIM2 */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; /* Enable NVCI Channel for TIM */
	NVIC_Init(&NVIC_InitStructure);

	NVIC_EnableIRQ(TIM2_IRQn); /* Enable NVCI for TIM */
	TIM_Cmd(TIM2, ENABLE); /* Enable TIM2 */
}
