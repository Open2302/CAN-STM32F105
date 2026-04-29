#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_flash.h"
#include <stdio.h>

// Прототипы функций
void RCC_Configuration(void);
void GPIO_Configuration(void);
void USART1_Configuration(void);
void USART1_SendString(const char* str);
void Delay_ms(uint32_t ms);

// Определение пина светодиода
// На многих платах (например, Blue Pill) светодиод встроенный на PC13
#define LED_PIN    GPIO_Pin_13
#define LED_PORT   GPIOC

int main(void)
{
    // Инициализация периферии
    RCC_Configuration();
    GPIO_Configuration();
    USART1_Configuration();

    // Изначально выключаем светодиод
    GPIO_ResetBits(LED_PORT, LED_PIN);

    while (1)
    {
        // Отправляем слово
        USART1_SendString("Pushkin\r\n");

        // Задержка между отправками (чтобы видеть мигание светодиода)
        // Уберите или уменьшите, если нужна максимальная скорость
        Delay_ms(1000);
    }
}

// Настройка тактирования
void RCC_Configuration(void)
{
    // Включаем тактирование:
    // GPIOA (для USART1 TX - пин PA9)
    // GPIOC (для светодиода - пин PC13)
    // USART1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_USART1, ENABLE);
}

// Настройка GPIO
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. Настройка пина USART1 TX (PA9)
    // Режим: Альтернативная функция, выход, Push-Pull
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 2. Настройка пина светодиода (PC13)
    // Режим: Выход, Push-Pull
    GPIO_InitStructure.GPIO_Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_PORT, &GPIO_InitStructure);
}

// Настройка USART1
void USART1_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

// Функция отправки строки с управлением светодиодом
void USART1_SendString(const char* str)
{
    // === ЗАЖИГАЕМ светодиод ===
    GPIO_SetBits(LED_PORT, LED_PIN);

    while (*str)
    {
        // Ждем, пока регистр данных не освободится (TXE)
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

        // Отправляем байт
        USART_SendData(USART1, *str);
        str++;
    }

    // Ждем окончания передачи последнего бита (TC)
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

    // === ГАСИМ светодиод ===
    GPIO_ResetBits(LED_PORT, LED_PIN);
}

// Простая задержка в миллисекундах
void Delay_ms(uint32_t ms)
{
    volatile uint32_t count;
    while (ms--)
    {
        for(count = 0; count < 8000; count++);
    }
}
