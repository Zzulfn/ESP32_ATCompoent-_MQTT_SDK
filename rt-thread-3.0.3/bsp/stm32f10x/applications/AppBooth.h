
#ifndef _APPBOOTH_H
#define _APPBOOTH_H

#define STBY_ON        GPIO_SetBits(STBY_gpio , STBY_pin)
#define STBY_OFF       GPIO_ResetBits(STBY_gpio , STBY_pin)

#define MUTE_ON        GPIO_SetBits(MUTE_gpio , MUTE_pin)
#define MUTE_OFF       GPIO_ResetBits(MUTE_gpio , MUTE_pin)

#define GAIN0_ON        GPIO_SetBits(GAIN0_gpio , GAIN0_pin)
#define GAIN0_OFF       GPIO_ResetBits(GAIN0_gpio , GAIN0_pin)

#define GAIN1_ON        GPIO_SetBits(GAIN1_gpio , GAIN1_pin)
#define GAIN1_OFF       GPIO_ResetBits(GAIN1_gpio , GAIN1_pin)


#define GAIN1_rcc                    RCC_APB2Periph_GPIOB
#define GAIN1_gpio                   GPIOB
#define GAIN1_pin                    (GPIO_Pin_12)

#define GAIN0_rcc                    RCC_APB2Periph_GPIOB
#define GAIN0_gpio                   GPIOB
#define GAIN0_pin                    (GPIO_Pin_13)

#define MUTE_rcc                    RCC_APB2Periph_GPIOB
#define MUTE_gpio                   GPIOB
#define MUTE_pin                    (GPIO_Pin_14)

#define STBY_rcc                    RCC_APB2Periph_GPIOB
#define STBY_gpio                   GPIOB
#define STBY_pin                    (GPIO_Pin_15)

extern void Booth_Thread_enter(void);


#endif 
