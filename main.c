#include "stm32f4xx_hal.h"
#include "usart.h"
#include "timer.h"
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <string.h>

#define ASCII_CLEAR_LINE     "\x1b[2K"
#define ASCII_MOVE_TO_START  "\x1b[1;1H"
#define ASCII_MOVE_TO_INPUT  "\x1b[10;1H"
#define ASCII_CLEAR_SCREEN   "\x1b[2J"
#define ASCII_MOVE_TO        "\x1b[%u;%uH"

#define ARR_SIZE     20

static uint8_t temp_arr[ARR_SIZE] = {20, 19, 18, 19, 21, 23, 20, 16, 18, 21,
                                     23, 24, 26, 25, 21, 20, 19, 21, 18, 20};
static uint16_t press_arr[ARR_SIZE] = {1013, 1011, 1007, 1010, 1005,
                                       1000, 1002, 1006, 1010, 1012,
                                       1013, 1015, 1017, 1014, 1012,
                                       1010, 1007, 1009, 1010, 1013};
static uint8_t string_buffer[64];

static uint32_t iter = 0;
static uint32_t display_line_index = 1;
static uint8_t state = 's';
uint16_t period = 0x03E8;


static void Display_Buffer_Append(const char * buf)
{
    char buffer[32];
    (void)snprintf(buffer, sizeof(buffer), ASCII_MOVE_TO, display_line_index++, 1);
    USART_WriteString(buffer);
    USART_WriteString(ASCII_CLEAR_LINE);
    USART_WriteString(buf);

    if (display_line_index > 5)
    {
        display_line_index = 1;
    }
}

static void Terminal_Clear(void)
{
    USART_WriteString(ASCII_CLEAR_SCREEN);
}

static void Terminal_Input_Append(char newchar, uint32_t pos)
{
    char buffer[32];
    (void)snprintf(buffer, sizeof(buffer), ASCII_MOVE_TO, 10, pos + 1);
    USART_WriteString(buffer);
    USART_PutChar(newchar);
}

static void Terminal_Input_Clear()
{
    USART_WriteString(ASCII_MOVE_TO_INPUT);
    USART_WriteString(ASCII_CLEAR_LINE);
}

static void Display_Help(void)
{
    display_line_index = 1;
    Display_Buffer_Append("If you want to display temperature enter 'temp' \n\r");
    Display_Buffer_Append("If you want to display pressure enter 'press' \n\r");
    Display_Buffer_Append("If you want to stop displaying enter 'stop' \n\r");
    Display_Buffer_Append("If you want to display list of awailable commands enter 'help' \n\r");
    Display_Buffer_Append("If you want to change display speed enter 'delay x', where x is an integer between 1 and 10 that expresses time in seconds \n\r");
}

static void Input_Parse(void)
{
    if ( 0 == strcmp((const char *)string_buffer, "temp"))
    {
        state = 't';
    }
    else if ( 0 == strcmp((const char *)string_buffer, "press"))
    {
        state = 'p';
    }
    else if ( 0 == strcmp((const char *)string_buffer, "stop"))
    {
        state = 's';
    }
    else if ( 0 == strcmp((const char *)string_buffer, "help"))
    {
        Display_Help();
    }
    else if ( 0 == strncmp((const char *)string_buffer, "delay ", 6))
    {
        int value = atoi((const char *)&string_buffer[6]);
        if (value >= 1 && value <= 10)
        {
            period = 1000*value;
        }
        else
        {
            Display_Buffer_Append("Invalid number \n\r");
        }
    }
    else
    {
        Display_Buffer_Append("Incorrect input \n\r");
    }
    memset(string_buffer, 0, sizeof(string_buffer));
}

static void Sensor_Process(void)
{
    char buffer [100];

    if ( state == 't')
    {
        (void)snprintf ( buffer, 100, "Temperature: %d st. C \n\r", temp_arr[iter++]);
        Display_Buffer_Append(buffer);
    }
    else if ( state == 'p')
    {
        (void)snprintf ( buffer, 100, "Pressure: %d st. C \n\r", press_arr[iter++]);
        Display_Buffer_Append(buffer);
    }
    else if ( state == 's')
    {

    }

    if (iter == ARR_SIZE - 1)
    {
        iter = 0;
    }
}

void Timer_Callback(void)
{
    Sensor_Process();
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

int main(void)
{
    HAL_Init();
    USART_Init();
    TIM_Init(Timer_Callback);
    Terminal_Clear();
    Display_Help();
    char c;
    int index = 0;
    while (1)
    {
        if (USART_GetChar(&c))
        {
            string_buffer[index] = c;
            Terminal_Input_Append(c, index);
            index ++;
            if (c == '\r')
            {
                string_buffer[index-1] = '\0';
                Input_Parse();
                Terminal_Input_Clear();
                index = 0;
            }
        }
    }
} /* main */
