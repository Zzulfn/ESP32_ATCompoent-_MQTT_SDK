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
**  Function:	����ģ����������
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


//--------------------------����ģ��ͨѶ�ܽŶ���----------------------------

#define VOICE_CS_H      GPIO_SetBits(VoiceClk_gpio, VoiceClk_pin)
#define VOICE_CS_L      GPIO_ResetBits(VoiceClk_gpio, VoiceClk_pin)

#define VOICE_SCL_H     GPIO_SetBits(VoiceCS_gpio, VoiceCS_pin)
#define VOICE_SCL_L     GPIO_ResetBits(VoiceCS_gpio, VoiceCS_pin)

#define VOICE_SDA_H     GPIO_SetBits(VoiceData_gpio, VoiceData_pin)
#define VOICE_SDA_L     GPIO_ResetBits(VoiceData_gpio, VoiceData_pin)

#define VOICE_ON        GPIO_SetBits(VoiceRest_gpio , VoiceRest_pin)
#define VOICE_OFF       GPIO_ResetBits(VoiceRest_gpio , VoiceRest_pin)

#define GetVoiceBusyStatus  GPIO_ReadInputDataBit(VoiceBusyCS_gpio, VoiceBusyCS_pin)

//---------------------------����ģ���е�FLASHоƬ�ܽŲ���-------------------


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
//FLASHоƬ�ͺ�W25Q80

#define	VF_MAXADDR						0x100000UL		//����ֽڵ�ַ��
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
* ����(Name)	  : void Set_VoiceLevel(uchar8 VoiceLvel)
* ����(Function)  : �������������ȼ�
* ����(Parameter) : VoiceLvel --���������ȼ�0-7
* ����(Variable)  : ��
* ����(Error)	  : ��
* ����(Return)	  : ��
* �޶�(Amendment) : Asen	2013.10.28
* ����(Other)	  : ��
***************************************************************************************************/
extern void Set_VoiceLevel(uchar8 VoiceLvel);
	
/**************************************************************************************************
* ����(Name)	  : void Voice_Play(uchar8 VoiceAddr)
* ����(Function)  : ��������
* ����(Parameter) : VoiceAddr --������ַ(0~219)
* ����(Variable)  : ��
* ����(Error)	  : ��
* ����(Return)	  : ��
* �޶�(Amendment) : Asen 2013.05.03
* ����(Other)	  : ��
***************************************************************************************************/
extern void Voice_Play(uchar8 VoiceAddr);

/**************************************************************************************************
* ����(Name)	  : void Voice_Init(void)
* ����(Function)  : ����ģ���ʼ��
* ����(Parameter) : NULL
* ����(Variable)  : ��
* ����(Error)	  : ��
* ����(Return)	  : ��
* �޶�(Amendment) : gy
* ����(Other)	  : ��
***************************************************************************************************/
extern void Voice_Init(void);




/*����������������߸��������ļ���FLASH������������������*/

/******************************************************************************
* ����(Function) 	����FLASHдʹ��
*******************************************************************************/
extern void	VF_WriteEnable(void);

/******************************************************************************
* ����(Function) 	���ر�FLASHдʹ��
*******************************************************************************/
extern void	VF_WriteDisable(void);

/******************************************************************************
* ����(Function) 	������FLASH���ݡ�
*******************************************************************************/
extern void	VF_EraseChip(void);

/******************************************************************************
* ����(Name)	  	: void	VF_SectorErase(uint32 addr)
* ����(Function) 	������FLASHָ����ַ����sector���ݡ�
* ����(Parameter)	��uint32 addr
* ����(Return)	 	��NULL
*******************************************************************************/
extern void	VF_SectorErase(uint32 addr);

/******************************************************************************
* ����(Name)	  	: void	VF_PageProgram(uint32 addr, uchar8* sbuff, int32 slen)
* ����(Function) 	�����������ַ��Flashд�����ݡ�
* ����(Parameter)	��uint32 addr, uchar8* sbuff, int32 slen
* ����(Return)	 	������0�ɹ�,��0ʧ�ܡ�
*******************************************************************************/
extern uchar8	VF_PageProgram(uint32 addr, uchar8* sbuff, uint32 slen);

/******************************************************************************
* ����(Name)	  	: uchar8	VF_FastReadDualData(uint32 addr, uchar8* rbuff, uint32 rlen)
* ����(Function) 	����ͨ�������ݡ�
* ����(Parameter)	��uint32 addr, uchar8* sbuff, int32 slen
* ����(Return)	 	������0�ɹ�,��0ʧ�ܡ�
*******************************************************************************/
extern uchar8	VF_FastReadDualData(uint32 addr, uchar8* rbuff, uint32 rlen);

/*************************************************************************************
* ����(Name)	  : int  UpdateVoice_ToFlash(uint32 Addr, uchar8* pRecordBuf, uint32 Len)
* ����(Function)  : д������FLASH -- �Զ��жϿ�ҳд (1�����д128Byte)
* ����(Parameter) : ���׼�¼
* ����(Variable)  : ��
* ����(Error)	  : ��
* ����(Return)	  : 0 �ɹ�����0 ʧ��
* �޶�(Amendment) : Asen 	2013/04/28
* ����(Other)	  : ��
**************************************************************************************/
extern int  UpdateVoice_ToFlash(uint32 Addr, uchar8* pRecordBuf, uint32 Len);

/******************************************************************************
* ����(Name)	  	��void	VoiceModule_Restore(void)
* ����(Function) 	�������������ļ���ʹ���عܽŴ�������״̬�������������ģ��
* ����(Variable)	��NULL
* ����(Error)		��NULL
* ����(Parameter)	��NULL
* ����(Return)	 	��0--FLASH������ 0xFF--FLASH�쳣
* �޶�(Amendment)	��Asen	  2013.10.30
* ����(Other)		��NULL
*******************************************************************************/
extern void VoiceModule_Restore(void);

/******************************************************************************
* ����(Name)	  	��uchar8	VoiceFlash_Init(void)
* ����(Function) 	������ģ��FlashоƬ��ʼ��
* ����(Variable)	��NULL
* ����(Error)		��NULL
* ����(Parameter)	��NULL
* ����(Return)	 	��0--FLASH������ 0xFF--FLASH�쳣
* �޶�(Amendment)	��Asen	  2013.10.30
* ����(Other)		��NULL
*******************************************************************************/
extern uchar8 VoiceFlash_Init(void);


#endif
/*========================File end=================================================================*/


