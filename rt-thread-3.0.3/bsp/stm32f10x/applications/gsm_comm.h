
#ifndef _GSM_COMM_H_
#define _GSM_COMM_H_  

#include "stm32f10x_gpio.h"
#include "stm32f10x_it.h"
#include <rtthread.h>

#define MAXRXNUM 512

#define GSMPow_rcc                    RCC_APB2Periph_GPIOC
#define GSMPow_gpio                   GPIOC
#define GSMPow_pin                    (GPIO_Pin_3)

#define GSMRestart_rcc                RCC_APB2Periph_GPIOA
#define GSMRestart_gpio               GPIOA
#define GSMRestart_pin                (GPIO_Pin_5)


#define GSMWakeUp_rcc                RCC_APB2Periph_GPIOA
#define GSMWakeUp_gpio               GPIOA
#define GSMWakeUp_pin                (GPIO_Pin_5)


#define GSMPOW_ON        GPIO_SetBits(GSMPow_gpio,GSMPow_pin)
#define GSMPOW_OFF       GPIO_ResetBits(GSMPow_gpio,GSMPow_pin)

#define GSMRest_ON       GPIO_ResetBits(GSMRestart_gpio,GSMRestart_pin)
#define GSMRest_OFF      GPIO_SetBits(GSMRestart_gpio,GSMRestart_pin)
#define BaudRateUart2    115200

#define ReceiveDataHandled   0
#define ReceiveFistHeard     1
#define ReceiveSecondHeard   2
#define ReceiveOnePackedData 3
#define ReceiveOverTime      100000

#define MaxTryNum           5
#define OverTimes       60
#define OneOverTime         1000


#define MAXVOICENUM    5



//GSM~{A,=S~}
#define POWER_REST     0
#define TEST_AT_COMM   1
#define ColseEcho      2
#define SET_Baudrate   3 
#define SetWiFiMode    4
#define ConnectGW      5
#define ReadIPAddr     6
#define ConnectServer  7
#define SetTranMode    8
#define StartSendData  9
#define ConnectServerOK 10
#define RegisterDeviceOK 11


#define  AT_Commend       "AT" //1
#define  AT_CloseEcho     "ATE0"//2
#define  AT_BaudRate      "AT+UART=115200,8,1,0,0"//3
#define  AT_SetWiFiMode   "AT+CWMODE=3"//4
#define  AT_ConnectGW     "AT+CWJAP=\"HMDJ\",\"88888888\""//5
#define  AT_ReadIPAddr    "AT+CIFSR"//6
#define  AT_ConnectServer "AT+CIPSTART=\"TCP\",\"183.230.40.33\",80"//7
#define  AT_SetTranMode   "AT+CIPMODE=1"//8~{M84+D#?i~}
#define  AT_StartSendData "AT+CIPSEND"//9~{7"KM=SJUJ}>]~}
#define  AT_LeaveTranMode "+++"
#define  AT_CIPCLOSE      "AT+CIPCLOSE"


#define TOKEN  "duozhuo.com"


//OneNet 
#define DefaultServerIP "183.230.40.39" 
#define DefaultServerPort "6002"
#define DefaultDeviceID "506020095"
#define DefaultProductID "196724"
#define DefaultCecretCode "Device"
#define SUBCRIBE   "Device"
#define OneNet_Protocol  "MQTT"


#define OneNet_ConnectComm    0x10 //
#define OneNet_ConnectCommACK 0x20 //
#define OneNet_DisConnectComm 0x14 //
#define OneNet_PingComm       0xC0 //
#define OneNet_PingCommACK    0xD0 //
#define OneNet_SUBCRIBERComm  0x80 //
#define OneNet_SUBSCRIBECommACK   0x90//
#define OneNet_UNSUBSCRIBEComm    0xA2//
#define OneNet_UNSUBSCRIBEACKComm 0x30
typedef struct _ONENETDEVICE
{
	char pSeverIp[30];
	char ServerPort[10];
	char OneNet_DeviceID[20];
	char OneNet_ProductID[20];
	char CecretCode[20];
	rt_mutex_t OneNet_DeviceLock;
}stOneNetDevice,*pOneNetDevice;


#pragma pack(1)

/***************************~{PDLx~}***********************/
typedef struct _HEARTBEATREC
{
   unsigned short ProTocolHeard;
   unsigned short ProTocolLen;
   unsigned short ProTocolOrder;
   unsigned short ProTocolStation;
   unsigned char ProTocolCheckSum;
   unsigned short ProTocolTail;
}stHeartBeatRX,*pHeartBeatRX;

typedef struct _HEARTBEATTX
{
   unsigned short ProTocolHeard;
   unsigned short ProTocolLen;
   unsigned short ProTocolOrder;
   unsigned short ProTocolStation;
   unsigned char  SIMCardID[8];
   unsigned char  ProTocolCheckSum;
   unsigned short ProTocolTail;
}stHeartBeatTX,*pHeartBeatTX;

/**************************************************/


/********************************~{W"2a~}*****************/
typedef struct _DEVICEREGIST
{
   unsigned short ProTocolHeard;
   unsigned short ProTocolLen;
   unsigned short ProTocolOrder;
   unsigned short ProTocolStation;
   unsigned char  SIMID[8];
   unsigned int   JinDu;
   unsigned int   WeiDu;
   unsigned char  TOKEN1[11]; 
   unsigned char ProTocolCheckSum;
   unsigned short ProTocolTail;
}stDeviceRegist,*pDeviceRegist;

/**************************************************/



/**************************~{;qH!2NJ}~}*****************/

typedef struct _USERPAYPARM
{
   unsigned int   Money;//~{=p6n~}
   unsigned char  MoneyType;//~{V'3v7=J=~}
   unsigned char  PayType;//~{V'867=J=~}
   unsigned int   UserID;//~{SC;'~}ID,
   unsigned int   SellerID;//~{IL;'~}ID
}stUserPayParam,*pUserPayParam;


typedef struct _VOICPALY
{
  stUserPayParam PalyParam[MAXVOICENUM];
  unsigned char  HeardIndex;// 1~{;9C;SP~}
  unsigned char  TailIndex;
}stVoicePaly,*pVoicePaly;

typedef struct _GETDATA
{
   unsigned short ProTocolHeard;//
   unsigned short ProTocolLen;//
   unsigned short ProTocolOrder;//
   unsigned short ProTocolStation;//
   stUserPayParam UserPayParam;
   unsigned char  ProTocolCheckSum;
   unsigned short ProTocolTail;
}stGetData,*pGetData;



typedef struct _SENTPARAM
{
   unsigned short ProTocolHeard;//
   unsigned short ProTocolLen;//
   unsigned short ProTocolOrder;//
   unsigned short ProTocolStation;//
   unsigned char  SIMID[8];
   stUserPayParam UserPayParamBack;
   unsigned char  ProTocolCheckSum;
   unsigned short ProTocolTail;
}stSENTPARAM, *pSENTPARAM;


/**************************************************/
#pragma pack()

typedef struct _HEARTOVERTIME
{
   	unsigned char HeartBeartWaitTimes;
	unsigned char HeartBWTStartMark;
	unsigned int LastHeartTick;
	unsigned int CurrentHeartTick;
}stHeartOverTime,*pHeartOverTime;

typedef struct _PROTOCOL
{
  unsigned short ProtocolOrder;
  unsigned char  (*pFunction)(unsigned char* pDataBuf, unsigned int Len);
} stProtocol,*protocol;

typedef struct _RECEIVER
{
    unsigned char  *pRXBuf;
	unsigned short  RXBufSize;
    unsigned int   RX_index;//the leastest Recevie Num
    unsigned char  ReceiveStation;//0 ~{4&@mMj3I#,~}1~{=SJU5=5ZR;8vM7#,~} 2~{=SJU5=5Z6~8vM7~}  3 ~{=SJUMj3I~}
	unsigned char  ReceiveEnable;
	unsigned char  FirstHeard;
	unsigned char  SecondHeard;
	unsigned char  FirstTail;
	unsigned char  SecondTail;
	unsigned int   ReceiveStartTime;
	
}stECEIVE,*pRECEIVE;

typedef struct _GSMCONNECT
{
	unsigned char ConnectStation;
	unsigned char TryTimes;

} stGSMConnect,*pGSMConnect;

typedef struct _SYSTEMPARA
{ 
  unsigned char RegistMark;
           char SIMID[20];
  unsigned char DeviceID[8];
  unsigned int  WeiDu;
  unsigned int  JinDu;
 
}stSystemPara,*pSystemPara;


extern rt_device_t DevNew;
extern stVoicePaly VoicePlay;
extern stECEIVE ATRX;	 //AT Recevie Struct
extern stECEIVE GSM_TransparentRX; // Transparent RX Struct

extern void GSM_thread_entry(void);
extern void GSMRXCallBack(unsigned char ReceiveByteData);
extern void TLV_GetParam(void);



#endif 

