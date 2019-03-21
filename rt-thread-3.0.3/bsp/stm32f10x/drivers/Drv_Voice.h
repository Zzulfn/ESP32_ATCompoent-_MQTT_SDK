/*====================================================================================
==                                                                                              
**  Copyright(c) 2013-07  Y&C Technology Corp. All rights reserved. 
==                                                                                           
**	Author:		Asen                                                                     
==                                                                                          
**  Version:	V1.0.0                                                                          
==
**  Filename:	Drv_voice.h                                                                       
==                                                                                         
**  Function:	语音模块驱动程序
==
**  Data:       2013-07-16                                                                       
=====================================================================================*/

#ifndef	_DRV_VOICE_H_
#define	_DRV_VOICE_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define uchar8 unsigned char
#define uint32 unsigned int 


//--------------------------语音模块通讯管脚定义----------------------------

#define VOICE_CS_H      GPIO_SetBits(VoiceClk_gpio, VoiceClk_pin)
#define VOICE_CS_L      GPIO_ResetBits(VoiceClk_gpio, VoiceClk_pin)

#define VOICE_SCL_H     GPIO_SetBits(VoiceCS_gpio, VoiceCS_pin)
#define VOICE_SCL_L     GPIO_ResetBits(VoiceCS_gpio, VoiceCS_pin)

#define VOICE_SDA_H     GPIO_SetBits(VoiceData_gpio, VoiceData_pin)
#define VOICE_SDA_L     GPIO_ResetBits(VoiceData_gpio, VoiceData_pin)

#define VOICE_ON        GPIO_SetBits(VoiceRest_gpio , VoiceRest_pin)
#define VOICE_OFF       GPIO_ResetBits(VoiceRest_gpio , VoiceRest_pin)

#define GetVoiceBusyStatus  GPIO_ReadInputDataBit(VoiceBusyCS_gpio, VoiceBusyCS_pin)

//---------------------------语音模块中的FLASH芯片管脚操作-------------------


#define VoiceClk_RCC                 RCC_APB2Periph_GPIOD  //
#define VoiceClk_gpio                GPIOD
#define VoiceClk_pin                 (GPIO_Pin_2)

#define VoiceCS_RCC                  RCC_APB2Periph_GPIOC
#define VoiceCS_gpio                 GPIOC
#define VoiceCS_pin                  (GPIO_Pin_12)

#define VoiceData_RCC                RCC_APB2Periph_GPIOC
#define VoiceData_gpio               GPIOC
#define VoiceData_pin                (GPIO_Pin_11)


#define VoiceRest_RCC                RCC_APB2Periph_GPIOB
#define VoiceRest_gpio             GPIOB
#define VoiceRest_pin              (GPIO_Pin_0)


#define VoiceBusy_RCC                RCC_APB2Periph_GPIOC
#define VoiceBusyCS_gpio             GPIOC
#define VoiceBusyCS_pin              (GPIO_Pin_10)


		



//----------------------------------------------W25Q80 Command definition---
//FLASH芯片型号W25Q80

#define	VF_MAXADDR						0x100000UL		//最大字节地址。
#define	VF_MAXBlockByte					(64*1024)
#define	VF_MAXSectorByte				(4*1024)
#define	VF_MAXPageByte					256

#define VF_Cmd_WriteEnable 				0x06
#define VF_Cmd_WriteDisable 			0x04
#define VF_Cmd_ReadStatusReg 			0x05
#define VF_Cmd_WriteStatusReg 			0x01
#define VF_Cmd_ReadData  				0x03
#define VF_Cmd_FastReadData 			0x0B
#define VF_Cmd_FastReadDualOutput		0x3B
#define VF_Cmd_PageProgram 				0x02
#define	VF_Cmd_BlockErase				0xd8  
#define	VF_Cmd_SectorErase				0x20  
#define VF_Cmd_ChipErase  				0xC7
#define VF_Cmd_PowerDown  				0xB9
#define VF_Cmd_ReleasePowerDown 		0xAB
#define VF_Cmd_DeviceID  				0xAB
#define VF_Cmd_ManufactDeviceID 		0x90
#define	VF_Cmd_JEDEC_ID					0x9f  




/**************************************************************************************************
* 函数(Name)	  : void Set_VoiceLevel(uchar8 VoiceLvel)
* 功能(Function)  : 设置语音音量等级
* 参数(Parameter) : VoiceLvel --语音音量等级0-7
* 变量(Variable)  : 无
* 错误(Error)	  : 无
* 返回(Return)	  : 无
* 修订(Amendment) : Asen	2013.10.28
* 其他(Other)	  : 无
***************************************************************************************************/
extern void Set_VoiceLevel(uchar8 VoiceLvel);
	
/**************************************************************************************************
* 函数(Name)	  : void Voice_Play(uchar8 VoiceAddr)
* 功能(Function)  : 播放语音
* 参数(Parameter) : VoiceAddr --语音地址(0~219)
* 变量(Variable)  : 无
* 错误(Error)	  : 无
* 返回(Return)	  : 无
* 修订(Amendment) : Asen 2013.05.03
* 其他(Other)	  : 无
***************************************************************************************************/
extern void Voice_Play(uchar8 VoiceAddr);

/**************************************************************************************************
* 函数(Name)	  : void Voice_Init(void)
* 功能(Function)  : 语音模块初始化
* 参数(Parameter) : NULL
* 变量(Variable)  : 无
* 错误(Error)	  : 无
* 返回(Return)	  : 无
* 修订(Amendment) : gy
* 其他(Other)	  : 无
***************************************************************************************************/
extern void Voice_Init(void);




/*★★★★★★★★★★★★在线更新语音文件至FLASH★★★★★★★★★★★★★★★★★*/

/******************************************************************************
* 功能(Function) 	：打开FLASH写使能
*******************************************************************************/
extern void	VF_WriteEnable(void);

/******************************************************************************
* 功能(Function) 	：关闭FLASH写使能
*******************************************************************************/
extern void	VF_WriteDisable(void);

/******************************************************************************
* 功能(Function) 	：擦除FLASH数据。
*******************************************************************************/
extern void	VF_EraseChip(void);

/******************************************************************************
* 函数(Name)	  	: void	VF_SectorErase(uint32 addr)
* 功能(Function) 	：擦除FLASH指定地址所在sector数据。
* 参数(Parameter)	：uint32 addr
* 返回(Return)	 	：NULL
*******************************************************************************/
extern void	VF_SectorErase(uint32 addr);

/******************************************************************************
* 函数(Name)	  	: void	VF_PageProgram(uint32 addr, uchar8* sbuff, int32 slen)
* 功能(Function) 	：可以任意地址向Flash写入数据。
* 参数(Parameter)	：uint32 addr, uchar8* sbuff, int32 slen
* 返回(Return)	 	：返回0成功,非0失败。
*******************************************************************************/
extern uchar8	VF_PageProgram(uint32 addr, uchar8* sbuff, uint32 slen);

/******************************************************************************
* 函数(Name)	  	: uchar8	VF_FastReadDualData(uint32 addr, uchar8* rbuff, uint32 rlen)
* 功能(Function) 	：又通道读数据。
* 参数(Parameter)	：uint32 addr, uchar8* sbuff, int32 slen
* 返回(Return)	 	：返回0成功,非0失败。
*******************************************************************************/
extern uchar8	VF_FastReadDualData(uint32 addr, uchar8* rbuff, uint32 rlen);

/*************************************************************************************
* 函数(Name)	  : int  UpdateVoice_ToFlash(uint32 Addr, uchar8* pRecordBuf, uint32 Len)
* 功能(Function)  : 写数据至FLASH -- 自动判断跨页写 (1次最多写128Byte)
* 参数(Parameter) : 交易记录
* 变量(Variable)  : 无
* 错误(Error)	  : 无
* 返回(Return)	  : 0 成功，非0 失败
* 修订(Amendment) : Asen 	2013/04/28
* 其他(Other)	  : 无
**************************************************************************************/
extern int  UpdateVoice_ToFlash(uint32 Addr, uchar8* pRecordBuf, uint32 Len);

/******************************************************************************
* 函数(Name)	  	：void	VoiceModule_Restore(void)
* 功能(Function) 	：更新完语音文件后使下载管脚处于输入状态，避免干扰语音模块
* 变量(Variable)	：NULL
* 错误(Error)		：NULL
* 参数(Parameter)	：NULL
* 返回(Return)	 	：0--FLASH正常， 0xFF--FLASH异常
* 修订(Amendment)	：Asen	  2013.10.30
* 其他(Other)		：NULL
*******************************************************************************/
extern void VoiceModule_Restore(void);

/******************************************************************************
* 函数(Name)	  	：uchar8	VoiceFlash_Init(void)
* 功能(Function) 	：语音模块Flash芯片初始化
* 变量(Variable)	：NULL
* 错误(Error)		：NULL
* 参数(Parameter)	：NULL
* 返回(Return)	 	：0--FLASH正常， 0xFF--FLASH异常
* 修订(Amendment)	：Asen	  2013.10.30
* 其他(Other)		：NULL
*******************************************************************************/
extern uchar8 VoiceFlash_Init(void);


#endif
/*========================File end=================================================================*/


