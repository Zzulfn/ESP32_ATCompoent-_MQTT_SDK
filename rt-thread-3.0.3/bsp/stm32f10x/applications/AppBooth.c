#include "AppBooth.h"
#include "stm32f10x_gpio.h"
#include <rtthread.h>

static void BoothGPIO_Init(void);

void Booth_Thread_enter(void)
{
      BoothGPIO_Init();
      while(1)
      {
        rt_thread_delay(100);
      }

}


static void BoothGPIO_Init(void)
{
          GPIO_InitTypeDef GPIO_InitStructure;
      
       RCC_APB2PeriphClockCmd(  GAIN1_rcc|GAIN0_rcc|MUTE_rcc|
                              STBY_rcc  ,ENABLE  );
       GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Pin   = STBY_pin|MUTE_pin|GAIN0_pin|GAIN1_pin;
       GPIO_Init(STBY_gpio, &GPIO_InitStructure);


       STBY_ON;
       MUTE_ON;
       GAIN0_ON;
       GAIN1_ON;

}

