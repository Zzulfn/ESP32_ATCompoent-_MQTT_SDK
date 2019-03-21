/*====================================================================================
==                                                                                              
**  Copyright(c) 2013-07  Y&C Technology Corp. All rights reserved. 
==                                                                                           
**	Author:		Asen                                                                     
==                                                                                          
**  Version:	V1.0.0                                                                          
==
**  Filename:	Drv_voice.c                                                                       
==                                                                                         
**  Function:	语音模块驱动程序
==
**  Data:       2013-07-16                                                                       
=====================================================================================*/

#include "Drv_Voice.h"
//#include "Drv_Sys.h"
#include "stm32f10x.h"




/**************************************************************************************************
* 函数(Name)	  : void Voice_Delay_150us(uint32 time)
* 功能(Function)  : 语音模块通讯延时函数
* 参数(Parameter) : 延时时间(单位150us)
* 变量(Variable)  : 无
* 错误(Error)	  : 无
* 返回(Return)	  : 无
* 修订(Amendment) : Asen	2013/05/03
* 其他(Other)	  : 无
***************************************************************************************************/
void Voice_Delay_150us(uint32 time)
{
	uint32 i, j;
	for(i=0; i<time; i++)
	{
		for(j=150; j>0; j--);
	}
}

/**************************************************************************************************
* 函数(Name)	  : void Voice_SendCmd(uchar8 cmd)
* 功能(Function)  : 对语音模块发送指令
* 参数(Parameter) : cmd --指令
* 变量(Variable)  : 无
* 错误(Error)	  : 无
* 返回(Return)	  : 无
* 修订(Amendment) : Asen	2013/05/03
* 其他(Other)	  : 无
***************************************************************************************************/
void Voice_SendCmd(uchar8 cmd)
{	
	uchar8 i;

//    VOICE_OFF;
//    Voice_Delay_150us(100);
    VOICE_ON;
    Voice_Delay_150us(100);
	VOICE_CS_L;
	Voice_Delay_150us(100);

	for(i=0; i<8; i++)
	{
		VOICE_SCL_L;
		if(cmd & 0x01) 
		{
			VOICE_SDA_H;
		}
		else
		{
			VOICE_SDA_L;
		}
		cmd >>= 1;
		Voice_Delay_150us(1);
		VOICE_SCL_H;
		Voice_Delay_150us(1);
	}
	VOICE_CS_H;
}

/**************************************************************************************************
* 函数(Name)	  : void Set_VoiceLevel(uchar8 VoiceLvel)
* 功能(Function)  : 设置语音音量等级
* 参数(Parameter) : VoiceLvel--语音音量等级0-7
* 变量(Variable)  : 无
* 错误(Error)	  : 无
* 返回(Return)	  : 无
* 修订(Amendment) : Asen	2013.10.28
* 其他(Other)	  : 无
***************************************************************************************************/
void Set_VoiceLevel(uchar8 VoiceLvel)
{
	if(VoiceLvel < 8)
	{
		VoiceLvel += 0xE0;
	}

	if((VoiceLvel >= 0xE0)&&(VoiceLvel < 0xE8))
	{
		Voice_SendCmd(VoiceLvel);	
	}
}

/**************************************************************************************************
* 函数(Name)	  : void Voice_Play(uchar8 VoiceAddr)
* 功能(Function)  : 播放语音
* 参数(Parameter) : VoiceAddr -- 语音地址(0~219)
* 变量(Variable)  : 无
* 错误(Error)	  : 无
* 返回(Return)	  : 无
* 修订(Amendment) : Asen	2013/05/03
* 其他(Other)	  : 无
***************************************************************************************************/
void Voice_Play(uchar8 VoiceAddr)
{	
	while(!GetVoiceBusyStatus);
    
	Voice_SendCmd(VoiceAddr);	
}

/**************************************************************************************************
* 函数(Name)	  : void Voice_Init(void)
* 功能(Function)  : 语音模块初始化
* 参数(Parameter) : NULL
* 变量(Variable)  : 无
* 错误(Error)	  : 无
* 返回(Return)	  : 无
* 修订(Amendment) : Asen	2013/05/03
* 其他(Other)	  : 无
***************************************************************************************************/
void Voice_Init(void)
{   
     GPIO_InitTypeDef GPIO_InitStructure;
    
     RCC_APB2PeriphClockCmd(  VoiceClk_RCC|VoiceCS_RCC|VoiceData_RCC|
                            VoiceRest_RCC|VoiceBusy_RCC ,ENABLE  );
     
    
     GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_InitStructure.GPIO_Pin   = VoiceClk_pin;
     GPIO_Init(VoiceClk_gpio, &GPIO_InitStructure);

     GPIO_InitStructure.GPIO_Pin   = VoiceCS_pin; 
     GPIO_Init(VoiceCS_gpio, &GPIO_InitStructure);
    
     GPIO_InitStructure.GPIO_Pin   = VoiceData_pin;
     GPIO_Init(VoiceData_gpio, &GPIO_InitStructure);


     GPIO_InitStructure.GPIO_Pin   = VoiceRest_pin ;
     GPIO_Init(VoiceRest_gpio, &GPIO_InitStructure);


     GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
     GPIO_InitStructure.GPIO_Pin   = VoiceBusyCS_pin ;
     GPIO_Init(VoiceBusyCS_gpio, &GPIO_InitStructure);

     
	VOICE_CS_H;
	VOICE_SCL_H;
	VOICE_SDA_H;
	
	VOICE_ON;
	
  Set_VoiceLevel(7);
	
	Voice_Play(0);
	
	Voice_Play(1);
	Voice_Play(2);
	Voice_Play(3);
	Set_VoiceLevel(7);
}




/*========================File end===================================================*/

