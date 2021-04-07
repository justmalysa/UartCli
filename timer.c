#include "timer.h"
#include "usart.h"
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"

/* Private definitions -------------------------------------------------------*/

/* Definition for TIMx clock resources */
#define TIMx                             TIM6
#define TIMx_CLK_ENABLE()                __TIM6_CLK_ENABLE()

#define TIMx_FORCE_RESET()               __TIM6_FORCE_RESET()
#define TIMx_RELEASE_RESET()             __TIM6_RELEASE_RESET()

/* Definition for TIMx's NVIC */
#define TIMx_IRQn                        TIM6_DAC_IRQn
#define TIMx_IRQHandler                  TIM6_DAC_IRQHandler

/* Private variables ---------------------------------------------------------*/

/* extern variables */
extern uint8_t state;
extern uint8_t temp_arr[ARR_SIZE];
extern uint16_t press_arr[ARR_SIZE];
extern uint16_t period;

/* TIM descriptor */
static TIM_HandleTypeDef TimHandle;

int iter = 0;

static void change_period(void)
{
    __HAL_TIM_SET_AUTORELOAD(&TimHandle, period);
}

void TIMx_IRQHandler(void)
{
    change_period();
    char buffer [100];
    if (__HAL_TIM_GET_FLAG(&TimHandle, TIM_FLAG_UPDATE))
    {
        if (__HAL_TIM_GET_IT_SOURCE(&TimHandle, TIM_IT_UPDATE))
        {
            if (state == 't')
            {
                (void)snprintf ( buffer, 100, "Temperature: %d st. C \n\r", temp_arr[iter++]);
                 USART_WriteString(buffer);
            }
            else if (state == 'p')
            {
                (void)snprintf ( buffer, 100, "Pressure: %d hPa \n\r", press_arr[iter++]);
                 USART_WriteString(buffer);
            }
        }
        __HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_UPDATE);

        if (iter == ARR_SIZE - 1)
        {
            iter = 0;
        }
    }
}


/**
  * This function configures the hardware resources used in this example: 
  *  - Peripheral's clock enable
  *  - NVIC configuration for TIM interrupt request enable
  */
void HAL_TIM_MspInit(TIM_HandleTypeDef *htim)
{
    /* Enable TIM6 clock */
    TIMx_CLK_ENABLE(); 

    /* Configure the NVIC for TIM */
    /* NVIC for TIM6 */
    HAL_NVIC_SetPriority(TIMx_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(TIMx_IRQn);
}

/**
  * This function frees the hardware resources used in this example:
  *   - Disable the Peripheral's clock
  *   - Revert NVIC configuration to their default state
  */
void HAL_TIM_MspDeInit(TIM_HandleTypeDef *htim)
{
    /* Reset peripherals */
    TIMx_FORCE_RESET();
    TIMx_RELEASE_RESET();

    /* Disable the NVIC for USART */
    HAL_NVIC_DisableIRQ(TIMx_IRQn);
}

bool TIM_Init(void)
{
    // configure TIM
    TimHandle.Instance                 = TIMx;

    TimHandle.Init.AutoReloadPreload   = TIM_AUTORELOAD_PRELOAD_ENABLE;
    TimHandle.Init.Period              = period;
    TimHandle.Init.Prescaler           = 0x3E80;
    TimHandle.Channel                  = HAL_TIM_ACTIVE_CHANNEL_1;

    HAL_TIM_MspInit(&TimHandle);

    if(HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
    {
        return false;
    }

    HAL_TIM_Base_Start_IT(&TimHandle); 

    return true;
}
