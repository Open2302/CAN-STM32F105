#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_flash.h" Î

#define HSE_VALUE    8000000UL
#define SYSCLK_FREQ  72000000UL

extern uint32_t SystemCoreClock;

void SystemClock_Config(void);
void TIM2_Config(void);
void GPIO_Config(void);

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
    else while(1);

    SystemCoreClock = SYSCLK_FREQ;
}

void TIM2_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_InitStruct = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_InitStruct.TIM_Prescaler = 7200 - 1;
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStruct.TIM_Period = 10000 - 1;
    TIM_TimeBaseInit(TIM2, &TIM_InitStruct);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_Cmd(TIM2, ENABLE);
}

void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

int main(void)
{
    SystemClock_Config();
    TIM2_Config();
    GPIO_Config();

    while (1)
    {
        if (TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) != RESET)
        {
            TIM_ClearFlag(TIM2, TIM_FLAG_Update);

            if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_15) == Bit_RESET)
                GPIO_SetBits(GPIOB, GPIO_Pin_15);
            else
                GPIO_ResetBits(GPIOB, GPIO_Pin_15);
        }
    }
}
