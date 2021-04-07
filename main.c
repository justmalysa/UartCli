#include "stm32f4xx_hal.h"
#include "usart.h"
#include "timer.h"
#include <ctime>
#include <cstdlib>


uint8_t state = 'n';
uint16_t period = 0x03E8;

uint8_t temp_arr[ARR_SIZE] = {20, 19, 18, 19, 21, 23, 20, 16, 18, 21,
                              23, 24, 26, 25, 21, 20, 19, 21, 18, 20};
uint16_t press_arr[ARR_SIZE] = {1013, 1011, 1007, 1010, 1005,
                                1000, 1002, 1006, 1010, 1012,
                                1013, 1015, 1017, 1014, 1012,
                                1010, 1007, 1009, 1010, 1013};


void SysTick_Handler(void)
{
    HAL_IncTick();
}

int main(void)
{
    HAL_Init();
    USART_Init();
    TIM_Init();
    USART_WriteString("If you want to display temperature press 't'\n\r");
    USART_WriteString("If you want to display pressure press 'p'\n\r");
    USART_WriteString("If you want to stop displaying press 'n'\n\r");
    USART_WriteString("If you want to increase the display time by one second press 'w' \n\r");
    USART_WriteString("If you want to decrease the display time by one second press 's' \n\r");
    char c;
    while (1)
    {
        if (USART_GetChar(&c))
        {
            if (c == 't' || c == 'p' || c == 'n')
            {
                state = c;
            }
            else if (c == 'w')
            {
                period += 0x03E8;
            }
            else if (c == 's')
            {
                period -= 0x03E8;
            }
        }
    }
} /* main */
