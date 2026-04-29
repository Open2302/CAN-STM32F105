#ifndef __CONFIGPERIPH_H
#define __CONFIGPERIPH_H

#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "misc.h"
#include "stm32f10x_tim.h"


void SystemClock_Config();
void USART_Config();
void LED_GPIO_Config();
void EXTI1_Config();
void TIM2_Config();


#endif
