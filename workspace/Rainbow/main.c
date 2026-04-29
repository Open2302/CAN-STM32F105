#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

const int Leds[] = {GPIO_Pin_10, GPIO_Pin_14, GPIO_Pin_15};
volatile uint32_t tick = 0;
volatile uint32_t lastTime = 0;

/* Function Prototypes */
void GPIO_Config(void);
void TIM2_Config(void);
void TIM2_IRQHandler(void);

void GPIO_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	 for (int i = 0; i < 3; ++i){
		 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15; /*GPIO_Pin_Leds[i];*/
		 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		 GPIO_Init(&GPIO_InitStructure);
	 }
}
void TIM2_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_InitStructure;

	TIM_InitStructure.TIM_Prescaler = 8 - 1;
	TIM_InitStructure.TIM_Period = 1000 -1;
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(&TIM_InitStructure);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_EnableIRQ(TIM2_IRQn, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update))
	{

	}
}
int main(void)
{

    while(1)
    {
    }
}
