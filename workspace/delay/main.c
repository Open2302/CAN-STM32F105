#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>

volatile uint32_t delay = 0;

void Timer(void)
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
}
void SysTick_Handler(void)
{
	delay++;
}
int main(void)
{

    while(1)
    {
    }
}
