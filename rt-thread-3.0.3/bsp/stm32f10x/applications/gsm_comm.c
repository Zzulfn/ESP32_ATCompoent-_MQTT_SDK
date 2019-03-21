#include <rtthread.h>
#include "Gsm_comm.h"
#include "stdio.h"
#include "usart.h"
#include <at.h>
#include "APP_OneNet.h"
#include "APP_BLE.h"



#define MAXPROTOCOL 10

unsigned char temp[128] ={0};
unsigned char BuffRX[MAXRXNUM] ={0};
unsigned char CommBuff[MAXRXNUM] = {0};
unsigned char TxBuf[128] ={0};

stProtocol ProTocol[MAXPROTOCOL] ={0};
stVoicePaly VoicePlay;

stOneNetDevice OneNetDevie;
static unsigned char ConnectOneNetStep = 0;

unsigned char TempRxBuf[128] = {0};

void GSMRX_ReceiveByteHandle(unsigned char ByteData);

void GSMRXCallBack(unsigned char ReceiveByteData);

static void RX_Loop(stECEIVE* RX_Struct,unsigned char ReceiveData);
static void GSM_ControlPin_Init(void);
unsigned char  RXStructInit(stECEIVE* RX,unsigned char FirstHeard,
                                 unsigned char SecondHeard,
                                 unsigned char FirstTail,
								 unsigned char SecondTail,
								 unsigned char *DataBuf,
								 unsigned short DataBufLen
								 );

static void GSMInit(void);
static void RX_Loop(stECEIVE* RX_Struct,unsigned char ReceiveData);
static int GSM_ConnectingLoop(void);
static int PHS8_Reboot(void);
static int  ReadSIM_ID(void);
static void TLV_HeartBeat(unsigned char* DataBuf, unsigned int Len);

static int WaitGSM_AT_Respones(char * GSMRepones,unsigned int Waitetime,unsigned char *ReceiveLen);
static void ReceiveDataHandle(void);
static void ReStartReceive(void);
static unsigned char CheckSum(unsigned char *DataBuf , unsigned int Len);
static void TLV_RegisetBack(unsigned char* DataBuf, unsigned int Len);
static void ProTocolRegist(void);
static void InitGSM_TransparentRX(void);
static void MakeDeviceID(unsigned char*SIMID);
static void WaitHeartOvertimeHandle(void);
static void HeartOverTimeCheck(void);
static int OneNet_PackSUBSCRIBEData(unsigned char *DataBuf,unsigned short DataBufLen);
static int OneNet_PackPingData(unsigned char *DataBuf,unsigned short DataBufLen);
static int MQTT_SUBSCRIBESent(char*TargetDevice,  unsigned char *DataBuf, unsigned char DataLen);
static int  ReadSIM_ID(void);
static int Wait_OneNet_Respones(unsigned char MQTTCOMM,unsigned int Waitetime,unsigned char *ReceiveLen);
static void EST32ConnectOneNetLoop(void);
static void OneNetDevieInit(void);

static void OneNet_RegisterDevice(void);

static  int DetachATClientthread(void);
static  int StartUpATClientthread(void);

 stGSMConnect	GSMConnect;
 stECEIVE ATRX;   //AT Recevie Struct
 stECEIVE GSM_TransparentRX; // Transparent RX Struct
 rt_device_t DevNew;
 stSystemPara Systempara = {0};
 stHeartOverTime  HeartOverTime = {0};
 stUserPayParam GetDataSt;

 unsigned char TestBuf[0x12] = {0x32,0x10, 0x00,0x06, 0x53,0x65, 0x72,0x76, 0x65,0x72,0x00,0x01,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};
 
 
 /*********************************************************************************************************
 * @brief	void RX_Loop(stECEIVE* RX_Struct,unsigned char ReceiveData)
 * @retval	 -1 Fail   0 connected
 */
void GSM_ControlPin_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(GSMPow_rcc|GSMRestart_rcc|GSMWakeUp_rcc,ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin   = GSMPow_pin;
    GPIO_Init(GSMPow_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GSMRestart_pin;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;//~{?*B)Jd3vUbQy>M2;;aN*~}0.8v
    GPIO_Init(GSMRestart_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GSMWakeUp_pin;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;//~{?*B)Jd3vUbQy>M2;;aN*~}0.8v
    GPIO_Init(GSMWakeUp_gpio, &GPIO_InitStructure);
}


/*********************************************************************************************************
* @brief   void RX_Loop(stECEIVE* RX_Struct,unsigned char ReceiveData)
* @retval   -1 Fail   0 connected
*/

unsigned char  RXStructInit(stECEIVE* RX,unsigned char FirstHeard,
                                 unsigned char SecondHeard,
                                 unsigned char FirstTail,
								 unsigned char SecondTail,
								 unsigned char *DataBuf,
								 unsigned short DataBufLen
								 )
{
	 //1) AT Receive struct init 
	// RX->pRXBuf = (unsigned char*)malloc(MAXRXNUM);
	RX->pRXBuf = DataBuf;
	RX->RXBufSize = DataBufLen;
	 if(RX->pRXBuf == NULL)
	 {
			rt_kprintf("malloc RXBuf Fail !\n");
			return 0xff;
	 }
	 
	 RX->ReceiveEnable = 1;//~{J9D\=SJU~}
	 RX->RX_index = 0;
	 RX->ReceiveStation =0;
	 RX->FirstHeard = FirstHeard;
	 RX->SecondHeard = SecondHeard;
	 RX->FirstTail = FirstTail;
	 RX->SecondTail = SecondTail;

	 memset(RX->pRXBuf, 0x00, MAXRXNUM);
	 
	 return 0;                  
}


/*********************************************************************************************************
* @brief   void RX_Loop(stECEIVE* RX_Struct,unsigned char ReceiveData)
* @retval   -1 Fail   0 connected
*/
void GSMInit(void)
{ 
 int result;
	
//Step1: ~{3uJ<;/~} RX~{=a99Le~}
	result = RXStructInit(&ATRX,0x0D,0x0A,0x0D,0x0A,BuffRX,MAXRXNUM);
	if(-1 == result)
	{
		rt_kprintf("ATRX StructInit Fail !\n");
	}
	ATRX.ReceiveEnable =0;
	
	result = RXStructInit(&GSM_TransparentRX,0x64,0x7A,0x7A,0x64,CommBuff,MAXRXNUM);
	if(-1 == result)
	{
		rt_kprintf("ATRX StructInit Fail !\n");
	}
	
//Step 2:  GPIO ~{3uJ<;/~}
	 GSM_ControlPin_Init();

	 GSMPOW_ON;//give Power
	 GSMRest_OFF;
	
	memset((unsigned char*)&GSMConnect,0x00, sizeof(stGSMConnect));

}


/*********************************************************************************************************
* @brief   void RX_Loop(stECEIVE* RX_Struct,unsigned char ReceiveData)
* @retval   -1 Fail   0 connected
*/
  
void GSMRXCallBack(unsigned char ByteData)
{ 
	static int i=0;
//	
//  if((ATRX.ReceiveEnable==1) && (ATRX.pRXBuf != NULL))
//	{
//		 RX_Loop(&ATRX,ByteData);
//	}
//	TempRxBuf
	if(GSMConnect.ConnectStation ==ConnectServerOK)
	TempRxBuf[i++]= ByteData;

	if(i==128)
	{
		i=0;
	}

	if((GSM_TransparentRX.ReceiveEnable==1) && (GSM_TransparentRX.pRXBuf != NULL))
	{ 
	
		RX_Loop(&GSM_TransparentRX,ByteData);
	}
 
}


unsigned char TempBuf[521];


/*********************************************************************************************************
* @brief   void RX_Loop(stECEIVE* RX_Struct,unsigned char ReceiveData)
* @retval   -1 Fail   0 connected
*/

void RX_Loop(stECEIVE* RX_Struct,unsigned char ReceiveData)
{  
	static unsigned int i=0;
    TempBuf[i]= ReceiveData;
	i++;
	if(i== 521)
	{
		i=0;
	}
//	rt_kprintf("%s\n",ReceiveData);
	switch(RX_Struct->ReceiveStation)
	{
		case ReceiveDataHandled:
		{
			if(ReceiveData == RX_Struct->FirstHeard)
			{ 
				RX_Struct->RX_index = 0;
				RX_Struct->pRXBuf[RX_Struct->RX_index] = ReceiveData;
				RX_Struct->RX_index++;
				RX_Struct->ReceiveStation = ReceiveFistHeard;
			}
			 break;
		}
		case ReceiveFistHeard:
		{ 
			if(ReceiveData == RX_Struct->SecondHeard)
			{
				RX_Struct->pRXBuf[RX_Struct->RX_index] = ReceiveData;
				RX_Struct->ReceiveStation = ReceiveSecondHeard;
				RX_Struct->RX_index++;
			}
			else
			{
			  RX_Struct->RX_index =0;
				RX_Struct->ReceiveStation = ReceiveDataHandled;
				
			}
			break;
		}
		case ReceiveSecondHeard:
		{ 
			
			if(RX_Struct->RX_index < RX_Struct->RXBufSize)
			{
				
				RX_Struct->pRXBuf[RX_Struct->RX_index] = ReceiveData;
				if(ReceiveData ==RX_Struct->SecondTail && RX_Struct->pRXBuf[(RX_Struct->RX_index)-1] == RX_Struct->FirstTail)
				{
					RX_Struct->ReceiveStation = ReceiveOnePackedData;
				}
				RX_Struct->RX_index++;
			}
			else
			{
			
			}

			break;
		}
		case ReceiveOnePackedData:
		{
			break;
		}
		default :
			break;
	}

		
}

/*********************************************************************************************************
* @brief   GSM Connect Loop
* @retval   -1 Fail   0 connected
*/
int GSM_ConnectingLoop(void)
{
	
	unsigned char ReceiveLen = 0;
	int FindResult =0;
	static unsigned char TryNum = 0;
	unsigned int StartTick=0;
	unsigned char DelayTimes=0;
	int Result = -1;
	char TempATCMD[512] = {0};
	
		 switch(GSMConnect.ConnectStation)
		 {
		   //Step 0:  GSM ~{D#?iIO5gVXFt~}
			case POWER_REST:
			{   
				AT_COMPonet_Station = 1;
				rt_thread_delay(500);
				Result = ESP32_SendCOM_AT_CMD("AT+CIPCLOSE", "OK", 5, 0);
				rt_device_read(DevNew, 0,TempATCMD, sizeof(TempATCMD));
				GSMConnect.ConnectStation = TEST_AT_COMM; //
				break;
			}
			
		 //Step1:  GSM AT~{C|An2bJT~}
		   case TEST_AT_COMM:
			{       
					  
				Result = ESP32_SendCOM_AT_CMD(AT_Commend, "OK",100, 0);
				if(Result == 0)
				{   
					TryNum = 0;
					GSMConnect.ConnectStation = ColseEcho; //~{Hg9{=SJU5=~}SYSSTART~{Tr=xHk~}AT~{C|An2bJT~}
					rt_kprintf("Step1:AT Test OK !\n");
				}
				else
				{
					TryNum++;
					if(TryNum > MaxTryNum)
					{ 
						TryNum = 0;
						GSMConnect.ConnectStation = POWER_REST;
						rt_kprintf("Try Over Times Return !\n");
							  
						return -1;//~{3,9}VXJT4NJ}Tr75;X4&@mOBCf5DJBGi:sTY@4VXJT~}
					}
				}
						
					break;
			}
				
		 //Step2: ~{9X1U;XOT~}	
				case ColseEcho:
				{   
					Result = ESP32_SendCOM_AT_CMD(AT_CloseEcho, "OK", 5, 0);
					if(Result == 0)
					{  
					   TryNum = 0;
						GSMConnect.ConnectStation = SET_Baudrate ; //~{Hg9{=SJU5=~}SYSSTART~{Tr=xHk~}AT~{C|An2bJT~}
						rt_kprintf("Step2:Close The Echo Successful!\n");
					}
					else
					{
						TryNum++;
						if(TryNum >= MaxTryNum)
						{    
							 TryNum =0;
							 GSMConnect.ConnectStation = POWER_REST;
							 rt_kprintf("Try Over Times Return !\n");
							  
							 return -1;//~{3,9}VXJT4NJ}Tr75;X4&@mOBCf5DJBGi:sTY@4VXJT~}
						}
					}

						break;
				}
		 //Step3: ~{IhVC2(LXBJ~}	
				case SET_Baudrate:
				{   
					Result = ESP32_SendCOM_AT_CMD(AT_BaudRate, "OK", 5, 0);
					if(Result == 0)
					{   
						TryNum = 0;
						GSMConnect.ConnectStation = SetWiFiMode; //~{Hg9{=SJU5=~}SYSSTART~{Tr=xHk~}AT~{C|An2bJT~}
						rt_kprintf("Step3:Set Baudrate Succefull!\n");
					}
					else
					{
						TryNum++;
						if(TryNum >= MaxTryNum)
						{    
							 TryNum = 0;
							 GSMConnect.ConnectStation = POWER_REST;
							 rt_kprintf("Try Over Times Return !\n");
							  
							 return -1;//~{3,9}VXJT4NJ}Tr75;X4&@mOBCf5DJBGi:sTY@4VXJT~}
						}
					}

						break;
				}
				
		//Step4: ~{IhVCD#?i9$WwD#J=~}1~{#:~}Station 2:AP 3:AP+Station
				case SetWiFiMode:
				{   
					Result = ESP32_SendCOM_AT_CMD(AT_SetWiFiMode, "OK", 5, 0);
					if(Result == 0)
					{   
						TryNum = 0;
						GSMConnect.ConnectStation = ConnectGW ; //~{Hg9{=SJU5=~}SYSSTART~{Tr=xHk~}AT~{C|An2bJT~}
						rt_kprintf("Step4:Set the WiFi mode Ok !\n");
						
					}
					else
					{
						TryNum++;
						if(TryNum >= MaxTryNum)
						{    
							 TryNum = 0;
							 GSMConnect.ConnectStation = POWER_REST;
							 rt_kprintf("Try Over Times Return !\n");
							  
							 return -1;//~{3,9}VXJT4NJ}Tr75;X4&@mOBCf5DJBGi:sTY@4VXJT~}
						}
					}
	
						break;
				}
	   //Step5: ~{6AH!2"1#4f~}SIMcard ID
				case ConnectGW:
				{   
			
				    memset(TempATCMD,0x00,sizeof(TempATCMD));
				    sprintf(TempATCMD, "AT+CWJAP=\"%s\",\"%s\"",OneNetParam.GW_Name  ,OneNetParam.GW_KEY);
					Result = ESP32_SendCOM_AT_CMD(TempATCMD, "OK", 1000, 0);
					if(Result == 0)
					{   
						TryNum = 0;
						GSMConnect.ConnectStation = ReadIPAddr ; //~{Hg9{=SJU5=~}SYSSTART~{Tr=xHk~}AT~{C|An2bJT~}
						rt_kprintf("Step5:Connect The Gateway Successful!\n");
						
					}
					else
					{  // LWIP_ASSERT(message, assertion)
						
						TryNum++;
						if(TryNum >= MaxTryNum)
						{    
							 TryNum = 0;
							 GSMConnect.ConnectStation = POWER_REST;
							 rt_kprintf("Try Over Times Return !\n");
							  
							 return -1;//~{3,9}VXJT4NJ}Tr75;X4&@mOBCf5DJBGi:sTY@4VXJT~}
						}
					}

						break;
				}
	   //Step6:  CheckOperator
				case ReadIPAddr:
				{   
					Result = ESP32_SendCOM_AT_CMD(AT_ReadIPAddr, "OK", 100, 0);
					if(Result== 0)
					{  
						TryNum = 0;
						GSMConnect.ConnectStation = ConnectServer ; //~{Hg9{=SJU5=~}SYSSTART~{Tr=xHk~}AT~{C|An2bJT~}
						rt_kprintf("Step6:Read Mode Ip Sucessful!\n");
					}
					else
					{  
						TryNum++;
						if(TryNum >= MaxTryNum)
						{    
							 TryNum = 0;
							 GSMConnect.ConnectStation = POWER_REST;
							 rt_kprintf("Try Over Times Return !\n");
							  
							 return -1;//~{3,9}VXJT4NJ}Tr75;X4&@mOBCf5DJBGi:sTY@4VXJT~}
						}
					}

						break;
				}

				
	  //Step7: AT_SetInteModer
				case ConnectServer:
				{   
                    memset(TempATCMD,0x00,sizeof(TempATCMD));
					sprintf(TempATCMD,"AT+CIPSTART=\"TCP\",\"%s\",%d",OneNetParam.MQTT_ADDR,OneNetParam.MQTT_Port);
					Result = ESP32_SendCOM_AT_CMD(TempATCMD, "OK", 200, 0);
					if(Result == 0)
					{ 
						TryNum = 0;
						AT_COMPonet_Station = 0;
						GSMConnect.ConnectStation = ConnectServerOK ; //~{Hg9{=SJU5=~}SYSSTART~{Tr=xHk~}AT~{C|An2bJT~}
						rt_kprintf("Step7:Connect to the server Succeful\n");
					}
					else
					{   
						Result = ESP32_SendCOM_AT_CMD(TempATCMD, "CONNECTED", 200, 0);
						if(Result == 0)
						{
							
							TryNum = 0;
							AT_COMPonet_Station = 0;
							GSMConnect.ConnectStation = ConnectServerOK ; //~{Hg9{=SJU5=~}SYSSTART~{Tr=xHk~}AT~{C|An2bJT~}
							rt_kprintf("Step7:Connect to the server Succeful\n");
						}
						else
						{	
							TryNum++;
							if(TryNum >= MaxTryNum)
							{     
								 TryNum = 0;
								 GSMConnect.ConnectStation = POWER_REST;
								 rt_kprintf("Try Over Times Return !\n");
								  
								 return -1;//~{3,9}VXJT4NJ}Tr75;X4&@mOBCf5DJBGi:sTY@4VXJT~}
							}
							
						}
						
						//PHS8_SendAT_Cmd("AT+CIPCLOSE\r\n");//~{Hg9{RTA,=STrOH6O?*A,=S~}

					}
						break;
				}

		//Step8: Set Tranmode 

				case SetTranMode:
				{   
					Result = ESP32_SendCOM_AT_CMD(AT_SetTranMode, "OK", 100, 0);
					if(Result == 0)
					{ 
						TryNum = 0;
						GSMConnect.ConnectStation = StartSendData ; //~{Hg9{=SJU5=~}SYSSTART~{Tr=xHk~}AT~{C|An2bJT~}
						rt_kprintf("Step8:Set the tran mode OK!\n");
					}
					else
					{
						TryNum++;
						if(TryNum >= MaxTryNum)
						{    
							 TryNum = 0;
							 GSMConnect.ConnectStation = POWER_REST;
							 rt_kprintf("Try Over Times Return !\n");
							  
							 return -1;//~{3,9}VXJT4NJ}Tr75;X4&@mOBCf5DJBGi:sTY@4VXJT~}
						}
					}
						break;
				}


	//Step9: CheckPacketDomainService
				case StartSendData:
				{   
					Result = ESP32_SendCOM_AT_CMD(AT_StartSendData, "OK", 50, 0);
					if(Result == 0)
					{ 
						TryNum = 0;
						
						AT_COMPonet_Station = 0;
						rt_kprintf("Step9: begin to Send data AT_COMPonet_Station = %d! \n", AT_COMPonet_Station);
						GSMConnect.ConnectStation = ConnectServerOK ; //~{Hg9{=SJU5=~}SYSSTART~{Tr=xHk~}AT~{C|An2bJT~}
						rt_thread_delay(100);
					}
					else
					{
						TryNum++;
						if(TryNum >= MaxTryNum)
						{    
							 TryNum = 0;
							 GSMConnect.ConnectStation = POWER_REST;
							 rt_kprintf("Try Over Times Return !\n");
							  
							 return -1;//
						}
					}
					break;
					
				}

		    default:
            {
                break;
            }
					
	}
	
}



/*********************************************************************************************************
* @brief    ~{5H4}6AH!~}GSM~{75;X5DJ}>]#,2"UR5=O`S&WV7{4.~}
* @retval   -1 Fail, 0~{3I9&~}
*/

static int PHS8_Reboot(void)
{
	//Step1:AT~{=SJU=a99Le3uJ<;/~}
	ATRX.ReceiveEnable =0;
	ATRX.ReceiveStation =0;
	ATRX.RX_index=0;
	
	//Step2:AT~{=SJU=a99Le3uJ<;/~}
	GSM_TransparentRX.ReceiveEnable =1;
	GSM_TransparentRX.ReceiveStation =0;
	GSM_TransparentRX.RX_index =0;
	
	//Step3:GSM~{A4=SW4L,VXVC~}
	
	GSMConnect.ConnectStation = POWER_REST;

	//Step4: ~{VXFt~}GSM~{D#?i~}
	GSMPOW_OFF;
	GSMRest_ON;
    rt_thread_delay(3000);
    GSMPOW_ON;
	rt_thread_delay(1000);
//	GSMRest_ON;
	rt_thread_delay(200);
    GSMRest_OFF;
	rt_thread_delay(500);
	return 0;
	
}



/*********************************************************************************************************
* @brief   
* @retval   
*/

int WaitGSM_AT_Respones(char * GSMRepones,unsigned int Waitetime,unsigned char *ReceiveLen)
{
	unsigned int TempTime;
	unsigned int CurrentTime;
	unsigned int Temp =0;
	int result;
    TempTime = Waitetime;

	if(TempTime < 100)
	{
		TempTime = 100; //
	}
    
	//Step1:~{;qH!O5M351G0J1~}
	CurrentTime = rt_tick_get();
	while((rt_tick_get()-CurrentTime) < TempTime)
	{
		rt_thread_delay(10);
	}
    
	if(DevNew != NULL)
	{
	  Temp = rt_device_read(DevNew,0, ATRX.pRXBuf, MAXRXNUM);
	}
    if(ReceiveLen != NULL)
    {
    	*ReceiveLen = Temp;
    }
	if( Temp != 0 )//~{3uJ<;/~}
	{  

	   result =u8u8(ATRX.pRXBuf, Temp, GSMRepones, rt_strlen(GSMRepones));
	   return result;//0 ~{3I9&~}   -1 Fail
	}

	return -1;
}


/*********************************************************************************************************
* @brief    ~{TZJ}WiVPUR5=D?1jJ}Wi~}
* @retval   -1 Fail, 0~{3I9&~}
*/
static int u8u8(uint8_t *des, uint16_t desLen, uint8_t *src, uint16_t srcLen)
{
    uint8_t *str_des = des;
    uint8_t *str_src = src;
    uint8_t *temp_des;
    uint8_t *temp_src;
    int	i, j;
    int	temp = desLen - srcLen;

    if(temp < 0)
    {
        return -1;
    }

    for(i = 0; i <= temp; i++)
    {
        if(*str_des != *str_src)
        {
            str_des++;
        }
        else
        {
            temp_des = str_des;
            temp_src = str_src;

            for(j = 0; j < srcLen; j++)
            {
                if(*temp_des != *temp_src)
                {
                    break;
                }

                temp_des++;
                temp_src++;
            }

            if(j == srcLen)	//???????
            {
                return 0;
            }

            str_des++;
        }
    }

    return -1;
}





/*********************************************************************************************************
* @brief    
* @retval   -1 Fail, 
*/

int GSM_Reboot(void)
{  
    return PHS8_Reboot();
}


/*********************************************************************************************************
* @brief    
* @retval   
*/
void GSM_thread_entry(void)
{  
  int result = -1;

  ProTocolRegist();
  OneNetDevieInit();
  AT_Compoent_Init(); 
  OneNetParaInit();
  
	DevNew = rt_device_find("uart2");

	if(DevNew == NULL)
	{
		rt_kprintf("find DevNew Fail !!\n");
		while(1);
	}

	
 
	while(1)
	{

		if(GSMConnect.ConnectStation  != ConnectServerOK )
		{  
			GSM_ConnectingLoop();
		}

		if(BLE_Station  != BLEOK  && GSMConnect.ConnectStation  == ConnectServerOK)
		{  
			APP_ESP32_Config_loop();
		}
		
		if(GSMConnect.ConnectStation == ConnectServerOK && OneNetParam.DeviceResgistMark == DeviceReGist_FAIL && BLE_Station  == BLEOK)
		{
			result = RegisterDevice(&OneNetParam);
			if(result == 0)
			{
				
				OneNetParam.DeviceResgistMark = DeviceRegist_OK;
			    memcpy(OneNetParam.MQTT_ADDR,&MQTT_IPAddr_Default[0],strlen(MQTT_IPAddr_Default) );
				OneNetParam.MQTT_Port = MQTT_ServerPort;
				GSMConnect.ConnectStation = POWER_REST;

				rt_kprintf("ESP32 Start connect to OneNet MQTT Server !!!\n");
				
			}
		}

		if(GSMConnect.ConnectStation  == ConnectServerOK && OneNetParam.DeviceResgistMark == DeviceRegist_OK )
		{
			EST32ConnectOneNetLoop();
		}
		   
		HeartOverTimeCheck();
		ReceiveDataHandle();
	    rt_thread_delay(10);
		
	}
	
}


/*********************************************************************************************************
* @brief   
* @retval   NULL
*/



/*********************************************************************************************************
* @brief    
* @retval   NULL
*/
void EST32ConnectOneNetLoop(void)
{
	int Len =0;
	unsigned char Databuf[126] = {0};
	int result = -1;
	static  unsigned char TyeNum = 0;
	static unsigned int OldTime = 0;
	unsigned int CurrentTime =0 ;
	
//Step1:
	switch(ConnectOneNetStep)
	{
		case 0: //~{A,=S~}OneNet
		{   
			result =  OneNet_Connect();
			if(result == 0)
			{   TyeNum = 0;
				ConnectOneNetStep = 1;
				rt_kprintf("Connect OneNet Succefull !\n");
			}
			else
			{
				TyeNum ++;
				if(TyeNum >5)
				{
					ConnectOneNetStep = 0;
					GSMConnect.ConnectStation = 0;
				}
			}
			
			break;
		}

		case 1:
		{
			result = OneNet_SUBSCRIBE("806");
			if(result == 0)
			{
				ConnectOneNetStep = 2;
				rt_device_write(DevNew,0,TestBuf, 0x12);
				rt_kprintf("send the test data !\n");
				rt_kprintf("SUBSCEIBE Successful !\n");
			}
			else
			{
				TyeNum ++;
				if(TyeNum >5)
				{
					ConnectOneNetStep = 0;
					GSMConnect.ConnectStation = 0;
				}
			}
			break;
		}
		
		case 2:
		{    
			break;
		}
	}
}

void HeartOverTimeCheck(void)
{  
  unsigned int Temp =0;
  unsigned int CurrentTick;

  
  CurrentTick = rt_tick_get();
  if(HeartOverTime.HeartBWTStartMark == 1)
  {
  	 if( (CurrentTick- HeartOverTime.LastHeartTick)  > OneOverTime)
  	 {  
  	    Temp = (CurrentTick - HeartOverTime.LastHeartTick)/OneOverTime;
  	 	HeartOverTime.HeartBeartWaitTimes += Temp ;

		HeartOverTime.LastHeartTick =  CurrentTick;

		if(HeartOverTime.HeartBeartWaitTimes > OverTimes)
		{
			WaitHeartOvertimeHandle();
		}
  	 }
  } 
}


void WaitHeartOvertimeHandle(void)
{
   GSMConnect.ConnectStation = POWER_REST;//~{=SJUPDLx3,J1Tr~}GSM ~{VXFt~}
   GSMConnect.TryTimes = 0;
 
   memset((unsigned char*)&Systempara, 0x00, sizeof(stSystemPara));
   memset((unsigned char*)&HeartOverTime, 0x00, sizeof(HeartOverTime));
   
   memset((unsigned char*)&ATRX.pRXBuf, 0x00, ATRX.RXBufSize);
   
   memset((unsigned char*)&GSM_TransparentRX.pRXBuf, 0x00, ATRX.RXBufSize);

}

/*********************************************************************************************************
* @brief    GPRS ~{=SJU4&@m~}
* @retval   -1 Fail, 0~{3I9&~}
*/

void ReceiveDataHandle(void)
{   
	 unsigned int CurrentTick =0;
	 unsigned char TempSum =0;
	 unsigned int DataLen =0;
	 unsigned short Comment = 0xffff;
	 unsigned int i =0;
	 
 //Step 1: ~{=SJU3,J1EP6O~} 
     if(GSM_TransparentRX.ReceiveStation == 0x00)
     {    ReStartReceive();
		     return ;//~{J}>]RQ>-4&@m~}     
     }
	 if(GSM_TransparentRX.ReceiveStation != 0x00 && GSM_TransparentRX.ReceiveStation != 0x03)//~{?*J<=SJUSVC;SP=SJUMj~}
	 {
	 	CurrentTick = rt_tick_get();//~{=SJU?*J<QSJ1#,SCSZWv=SJU3,J151=SJU5=P-RiM7:s1XPk5C~}1~{CkDZ=SJUMj#,7qTr4SPB=SJU~}
		if((CurrentTick - GSM_TransparentRX.ReceiveStartTime) > ReceiveOverTime) 
		  {   
		     ReStartReceive();
		     return ;//~{J}>]RQ>-4&@m~}     
          }
	 }
	 
      rt_kprintf("**********GSM***************\n");
	 for(i=0;i<GSM_TransparentRX.RX_index; i++)
	 {
	   rt_kprintf("%02x",GSM_TransparentRX.pRXBuf[i]);
	 }
	 rt_kprintf("*********************************\n");

	 if(GSM_TransparentRX.RX_index < 7 )
	 {  
	 	rt_kprintf("the num of data too min!\n");
		ReStartReceive();
		return ;
	 }
	 
//Step 2: ~{3$6H<l2b~}
     DataLen = GSM_TransparentRX.pRXBuf[2]*255+GSM_TransparentRX.pRXBuf[3];
     if(DataLen > GSM_TransparentRX.RX_index -7  || DataLen >GSM_TransparentRX.RXBufSize -7)
		 {	 
			ReStartReceive();
			return ;//~{J}>]RQ>-4&@m~}	   
		 }


//Step 3: CRC~{<l2b~}	 
	 TempSum = GSM_TransparentRX.pRXBuf[GSM_TransparentRX.RX_index-3];//
     if(TempSum != CheckSum((unsigned char *)&GSM_TransparentRX.pRXBuf[2], GSM_TransparentRX.RX_index-5))
    {	 
	   ReStartReceive();
	   return ;//~{J}>]RQ>-4&@m~}	   
	}

	 
     Comment = (GSM_TransparentRX.pRXBuf[4]*255+GSM_TransparentRX.pRXBuf[5]);
//Step 4:~{8y>]C|AnWV5wSCO`9X:/J}~}
     protocolHandle(Comment);
 
     ReStartReceive();
     return ;//~{J}>]RQ>-4&@m~}     

 	
}

/*********************************************************************************************************
* @brief    ~{Q0~}
* @retval   NULl
*/
int protocolHandle(unsigned short ProtocolOrder)
{
   unsigned char i=0;
   
   for(i=0; i<MAXPROTOCOL; i++)
   	{
   	  if(ProTocol[i].ProtocolOrder == ProtocolOrder)
   	  {   
   	      if(ProTocol[i].pFunction != NULL)
   	  	  ProTocol[i].pFunction(GSM_TransparentRX.pRXBuf, GSM_TransparentRX.RX_index-1);
   	  	  return 0;
   	  }
	  	
   	}
    return -1;
}  


/*********************************************************************************************************
* @brief    ~{3uJ<;/=a99LeW<18OB4N=SJU~}
* @retval   NULl
*/
void ReStartReceive(void)
{
  GSM_TransparentRX.ReceiveEnable = 1;//~{?IRT=xPP=SJU~}
  GSM_TransparentRX.ReceiveStartTime =0;
  GSM_TransparentRX.ReceiveStation = 0;
  GSM_TransparentRX.RX_index =0;
  memset((unsigned char*)GSM_TransparentRX.pRXBuf, 0x00, GSM_TransparentRX.RXBufSize);
}


/*********************************************************************************************************
* @brief    ~{:MP#Qi~}
* @retval   ~{P#QiV5~}
*/
unsigned char CheckSum(unsigned char *DataBuf , unsigned int Len)
{
  unsigned char Sum =0;
  unsigned int i =0;
  for(i=0 ; i<Len; i++)
  {
  	Sum += DataBuf[i];
  }

  return Sum;
}

/*********************************************************************************************************
* @brief    ~{2bJT~}GSM~{D#?i~} AT~{C{@{~}
* @retval   -1 Fail, 0~{3I9&~}
*/
void ProTocolRegist(void)
{
   ProTocol[8].ProtocolOrder = 0x0008;
   ProTocol[8].pFunction = TLV_HeartBeat;

   ProTocol[1].ProtocolOrder = 0x0001;
   ProTocol[1].pFunction =  TLV_RegisetBack ;

   ProTocol[7].ProtocolOrder = 0x0007;
   ProTocol[7].pFunction = TLV_GetParam;
}



/*********************************************************************************************************
* @brief    ~{2bJT~}GSM~{D#?i~} AT~{C{@{~}
* @retval   -1 Fail, 0~{3I9&~}
*/
void TLV_RegisetBack(unsigned char* DataBuf, unsigned int Len)
{
   Systempara.RegistMark =1;
   HeartOverTime.HeartBWTStartMark = 1;
   HeartOverTime.LastHeartTick = rt_tick_get();//~{;qH!O5M3J1VS#,SCSZ=SJUPDLx3,J1EP6O~}
   rt_kprintf("Device Regiest Scuessfull !\n");
}

/*********************************************************************************************************
* @brief    ~{2bJT~}GSM~{D#?i~} AT~{C{@{~}
* @retval   -1 Fail, 0~{3I9&~}
*/
void TLV_HeartBeat(unsigned char* DataBuf, unsigned int Len)
{  
   pHeartBeatTX HeartBeatTX;
   
   rt_kprintf(" Devices Receive the HeartBeat Packet !\n");
   
   HeartBeatTX = (pHeartBeatTX)TxBuf;
   HeartBeatTX->ProTocolHeard = 0x7A64;
   HeartBeatTX->ProTocolLen = 0x0c00;
   HeartBeatTX->ProTocolOrder = 0x0800;
   HeartBeatTX->ProTocolStation = 0x0000;
   HeartBeatTX->ProTocolTail = 0x647A;
   memcpy((unsigned char*)&HeartBeatTX->SIMCardID, (unsigned char*)Systempara.DeviceID, 8);

   HeartBeatTX->ProTocolCheckSum = CheckSum((unsigned char *)&HeartBeatTX->ProTocolLen,14);
   rt_kprintf("Devices TX The HeartBeat Back packet !\n");
   rt_device_write(DevNew,0,TxBuf,sizeof(stHeartBeatTX));
   
   rt_kprintf("~{P-RiM7~}:0x%04x\n",HeartBeatTX->ProTocolHeard);
   rt_kprintf("~{J}>]3$6H~}:0x%04x\n",HeartBeatTX->ProTocolLen);
   rt_kprintf("~{C|AnWV~}:0x%04x\n",HeartBeatTX->ProTocolOrder);
   rt_kprintf("~{M(Q6W4L,~}:0x%04x\n",HeartBeatTX->ProTocolStation);
   rt_kprintf("~{Ih18Ih18~}ID:%s\n",HeartBeatTX->SIMCardID);
   
   rt_kprintf("~{0|N2~}:0x%04x\n",HeartBeatTX->ProTocolTail);
   
   
} 


/*********************************************************************************************************
* @brief    ~{2bJT~}GSM~{D#?i~} AT~{C{@{~}
* @retval   -1 Fail, 0~{3I9&~}
*/

void TLV_GetParam(void)
{
	pGetData   GetData;
	pSENTPARAM SentParam;
    pUserPayParam pVoicePlayTemp = NULL;
    unsigned char Temp = 0; 
    unsigned char SuccessMark = 0xff;
	   
	rt_kprintf(" Devices Money TLVPacket !\n");

    Temp = VoicePlay.TailIndex;
    Temp ++;

    if(Temp == MAXVOICENUM)
    {
       Temp = 0; 
    }

    if(Temp == VoicePlay.HeardIndex)
    {
        SuccessMark = 1 ;//~{AtR;LuPE:E5D?U8q~} ~{PE:ERQBz~}
    }
    else
    {
         SuccessMark  = 0;
    
         pVoicePlayTemp = &VoicePlay.PalyParam[VoicePlay.TailIndex];
         VoicePlay.TailIndex++;
         if(VoicePlay.TailIndex == MAXVOICENUM)
         {
             VoicePlay.TailIndex = 0;
         }
     
    	  GetData = (pGetData)GSM_TransparentRX.pRXBuf;
    
    
    	  pVoicePlayTemp->Money =  GetData->UserPayParam.Money;
    	  pVoicePlayTemp->MoneyType = GetData->UserPayParam.MoneyType;
    	  pVoicePlayTemp->PayType = GetData->UserPayParam.PayType;
    	  pVoicePlayTemp->SellerID = GetData->UserPayParam.SellerID;
    	  pVoicePlayTemp->UserID = GetData->UserPayParam.UserID;
    
    	  rt_kprintf("Money = 0x%08x\n",GetDataSt.Money);
    	  rt_kprintf("CosumType = 0x%02x\n",GetDataSt.MoneyType);
    	  rt_kprintf("PayType  = 0x%02x\n",GetDataSt.PayType);
    	  rt_kprintf("SellerID  = 0x%08x\n",GetDataSt.SellerID);
    	  rt_kprintf("UserID  = 0x%08x\n",GetDataSt.UserID);
     }
    
      
	 SentParam  = (pSENTPARAM)TxBuf;
	 SentParam->ProTocolHeard = 0x7A64;
	 SentParam->ProTocolLen = 0x1A00;
	 SentParam->ProTocolOrder = 0x0700;
     
     if(SuccessMark == 1)
     {
        SentParam->ProTocolStation = 0x0100;
     }
     else
     {
         SentParam->ProTocolStation = 0x0000;
     }
	 
	 SentParam->ProTocolTail = 0x647A;
	 memcpy((unsigned char*)&SentParam->SIMID, (unsigned char*)Systempara.DeviceID, 8);
	 
     SentParam->UserPayParamBack.Money =  GetData->UserPayParam.Money;
	 SentParam->UserPayParamBack.MoneyType = GetData->UserPayParam.MoneyType;
	 SentParam->UserPayParamBack.PayType = GetData->UserPayParam.PayType;
	 SentParam->UserPayParamBack.SellerID = GetData->UserPayParam.SellerID;
	 SentParam->UserPayParamBack.UserID = GetData->UserPayParam.UserID;
	 
	 SentParam->ProTocolCheckSum = CheckSum((unsigned char *)&SentParam->ProTocolLen, 0x1C);
	 rt_device_write(DevNew,0,TestBuf, 0x12);

	 MQTT_SUBSCRIBESent("Server",TxBuf, sizeof(stSENTPARAM));

	  

}

/*********************************************************************************************************
* @brief    ~{2bJT~}GSM~{D#?i~} AT~{C{@{~}
* @retval   ~{Ih18~}ID~{993I#:~}"dz" + {~{#(~}sim~{?(:E~}-100010001~{#)~}*3}  SIMID[20]  Systempara.DeviceID 
*/
void MakeDeviceID(unsigned char*SIMID)
{
  
   uint64_t TempDeviceID=0;
   unsigned char TempSIMID[20];  
   unsigned char i=0;

   char * pTemp;

//~{0QWV7{W*;;3IJ}WV~}
   for(i=0; i<20; i++)
   {
   	 TempSIMID[i] = SIMID[i] - '0';
   }
   for(i=1;i<=15;i++)
   {
	  TempDeviceID += TempSIMID[20-i];

	  TempDeviceID *=10;
   }
	 TempDeviceID =TempDeviceID/10;
//   rt_kprintf("TempDeviceID = %d",TempDeviceID);

   TempDeviceID -= 100010001;
   TempDeviceID *=3;

   pTemp = (char *)&TempDeviceID;

   TempDeviceID = 0x647A000000000000+TempDeviceID ;

   for(i=0;i<8;i++)
   {
   	 Systempara.DeviceID[i] = pTemp[7-i];
   }
   
}

/*********************************************************************************************************
* @brief    ~{2bJT~}GSM~{D#?i~} AT~{C{@{~}
* @retval   ~{Ih18~}ID~{993I#:~}"dz" + {~{#(~}sim~{?(:E~}-100010001~{#)~}*3}  SIMID[20]  Systempara.DeviceID 
*/
void OneNetDevieInit(void)
{  
	memcpy((unsigned char *)&OneNetDevie.pSeverIp,(unsigned char *)DefaultServerIP,strlen(DefaultServerIP) );
	memcpy((unsigned char *)&OneNetDevie.ServerPort,(unsigned char *)DefaultServerPort,strlen(DefaultServerPort) );
	memcpy((unsigned char *)&OneNetDevie.OneNet_ProductID,(unsigned char *)DefaultProductID,strlen(DefaultProductID) );
	memcpy((unsigned char *)&OneNetDevie.CecretCode,(unsigned char *)DefaultCecretCode,strlen(DefaultCecretCode) );
	memcpy((unsigned char *)&OneNetDevie.OneNet_DeviceID,(unsigned char *)DefaultDeviceID,strlen(DefaultDeviceID) );

	OneNetDevie.OneNet_DeviceLock = rt_mutex_create("OneNetLock", 1);
	if(OneNetDevie.OneNet_DeviceLock== NULL)
	{
		rt_kprintf("Create OneNetDevie Fail !\n");
	}
	
}


/*********************************************************************************************************
* @brief     
* @retval   
*/
int OneNet_PackConnetData(char*ProtocolName, unsigned short KeepTime,unsigned char *DataBuf,unsigned short DataBufLen)
{
	int result = -1;
	unsigned short Len =0;
	unsigned short Index =0;
	
	if(DataBuf == NULL)
	{
		return result ;
	}

//~{9L6(1(M7#:~}connect ~{C|AnWV~}, ~{J}>]3$6H#:U{8v?I1dM7<SJ}>]5D3$6H~}
	DataBuf[0] = OneNet_ConnectComm;//~{C|AnWV~}
	DataBuf[1] = 0x00;//~{U{8v?I1dM7<SJ}>]5D3$6H~}
	

//~{?I1d1(M7#:~}
	DataBuf[2] = 0x00;//~{P-RiC{3$6H~}
	DataBuf[3] = 0x04;
	memcpy(&DataBuf[4],(unsigned char*)OneNet_Protocol,4);
	DataBuf[8] = 0x04;
	DataBuf[9] = 0xC2;
	DataBuf[10] = (KeepTime>>8)&0x00ff; //~{3,J15H4}J1<d~}
	DataBuf[11] = KeepTime&0x00ff;
    
//~{SPP'TX:IIh18~}ID
    Index = 12;
	if((Index+2+Len) > DataBufLen)
	{
		return result;
	}
	Len = rt_strlen(OneNetDevie.OneNet_DeviceID);
	DataBuf[Index++] = (Len>>8)&0x00ff; //~{3,J15H4}J1<d~}
	DataBuf[Index++] = (Len&0x00ff) ;
	memcpy(&DataBuf[14],(unsigned char *)&OneNetDevie.OneNet_DeviceID,Len);
	Index += Len;
	
//~{2zF7~}ID
	Len = rt_strlen(OneNetDevie.OneNet_ProductID);
	if((Index+2+Len) > DataBufLen)
	{
		return result;
	}
	DataBuf[Index++] = (Len>>8)&0x00ff; //~{3,J15H4}J1<d~}
	DataBuf[Index++] = (Len&0x00ff) ;
	memcpy(&DataBuf[Index],(unsigned char *)&OneNetDevie.OneNet_ProductID,Len);
	Index += Len;

	
//~{C\Bk~}
	Len = rt_strlen(OneNetDevie.CecretCode);
	if((Index+2+Len) > DataBufLen)
	{
		return result;
	}
	DataBuf[Index++] = (Len>>8)&0x00ff; //~{3,J15H4}J1<d~}
	DataBuf[Index++] = (Len&0x00ff) ;
	memcpy(&DataBuf[Index],(unsigned char *)&OneNetDevie.CecretCode,Len);
	Index += Len;
	
	DataBuf[1] = Index-2;

	return Index;//~{WnVUJ}>]3$6H~}
	
	
}

/*********************************************************************************************************
* @brief    ~{2bJT~}GSM~{D#?i~} AT~{C{@{~}  
* @retval   ~{Ih18~}ID~{993I#:~}"dz" + {~{#(~}sim~{?(:E~}-100010001~{#)~}*3}  SIMID[20]  Systempara.DeviceID 
*/
int OneNet_PackSUBSCRIBEData(unsigned char *DataBuf,unsigned short DataBufLen)
{
	int result = -1;
	unsigned short Len =0;
	unsigned short Index =0;
	
	if(DataBuf == NULL)
	{
		return result ;
	}   

//~{9L6(1(M7#:~}connect ~{C|AnWV~}, ~{J}>]3$6H#:U{8v?I1dM7<SJ}>]5D3$6H~}
	DataBuf[0] = OneNet_SUBCRIBERComm;//~{C|AnWV~}
	DataBuf[1] = 0x00;//~{U{8v?I1dM7<SJ}>]5D3$6H~}
	

//~{?I1d1(M7#:~}
	DataBuf[2] = 0x00;//~{P-RiC{3$6H~}
	DataBuf[3] = 0x0A;
    Len = rt_strlen(SUBCRIBE);
	DataBuf[4] = (Len>>8)&0x00ff;
	DataBuf[5] = Len&0x00ff;

	memcpy(&DataBuf[6],(unsigned char *)SUBCRIBE,Len);
	Index = Len+6;
	DataBuf[Index++] = 0x01;

	DataBuf[1] = Index-2;//~{U{8v?I1dM7<SJ}>]5D3$6H~}
	
	return Index;//~{WnVUJ}>]3$6H~}
	
	
}

/*********************************************************************************************************
* @brief    ~{2bJT~}GSM~{D#?i~} AT~{C{@{~}  
* @retval   
*/
int OneNet_PackPingData(unsigned char *DataBuf,unsigned short DataBufLen)
{
	int result = -1;
	
	if(DataBuf == NULL)
	{
		return result ;
	}

	DataBuf[0] = OneNet_PingComm;//
	DataBuf[1] = 0x00;//
	
	return 2;//
	
}




/*********************************************************************************************************
* @brief    ~{2bJT~}GSM~{D#?i~} AT~{C{@{~}
* @retval   ~{Ih18~}ID~{993I#:~}"dz" + {~{#(~}sim~{?(:E~}-100010001~{#)~}*3}  SIMID[20]  Systempara.DeviceID 
*/
int MQTTProtoclHandle(unsigned char MQTTCOMM,unsigned char *DataBuf,unsigned short DataBufLen)
{
	int result = -1;
	unsigned char TempComm = 0xff;

	if(DataBuf == NULL)
	{
		return result ;
	}
	
	TempComm = DataBuf[0];
	if(MQTTCOMM == TempComm)
	{
		switch (TempComm)
		{
			case OneNet_ConnectCommACK://~{=bNvA,=S75;XV8An~}
			{
				if(DataBuf[2] == 0x01 && DataBuf[3] == 0x00)
				{
					 result = 0;
				}
				break;
			}
			case OneNet_SUBSCRIBECommACK:
			{
				result = 0;
				break;
			}
			case OneNet_PingCommACK:
			{
				result = 0;
			}
			
		}

		
	}

	return result ;

}



/*********************************************************************************************************
* @brief    ~{5H4}6AH!~}GSM~{75;X5DJ}>]#,2"UR5=O`S&WV7{4.~}
* @retval   -1 Fail, 0~{3I9&~}
*/

int Wait_OneNet_Respones(unsigned char MQTTCOMM,unsigned int Waitetime,unsigned char *ReceiveLen)
{
	unsigned int TempTime;
	unsigned int Temp =0;
	int result = -1;
    TempTime = Waitetime;

	if(TempTime < 100)
	{
		TempTime = 100; //
	}
    
	//Step1:~{;qH!O5M351G0J1~}
	while(TempTime--)
	{
		rt_thread_delay(8);
	}
    
	if(DevNew != NULL)
	{
	  Temp = rt_device_read(DevNew,0, ATRX.pRXBuf, MAXRXNUM);
	}
	
  if(ReceiveLen != NULL)
  {
    	*ReceiveLen = Temp;
  }
	if( Temp != 0 )//~{3uJ<;/~}
	{  
	//~{4S=SJU5=5DJ}>]@oUR5=OkR*UR5DWV7{#,Hg9{UR5=Tr3uJ<;/=SJU75;X=SJU3I9&~}
	   result = MQTTProtoclHandle(MQTTCOMM,ATRX.pRXBuf,Temp );
	   return result;//0 ~{3I9&~}   -1 Fail
	}
	rt_kprintf("Recieve Len = %d\n", Temp);
	return result;
}

/*********************************************************************************************************
* @brief    ~{5H4}6AH!~}GSM~{75;X5DJ}>]#,2"UR5=O`S&WV7{4.~}
* @retval   -1 Fail, 0~{3I9&~}
*/
int MQTT_SUBSCRIBESent(char*TargetDevice,  unsigned char *DataBuf, unsigned char DataLen)
{
	int result = -1;
	unsigned char Lenth =0;
	unsigned char TempTxBuf[256] = {0};
	
	if((DataBuf == NULL) || (TargetDevice == NULL))
	{
		return result ;		
	}
	if(DataLen > 230)
	{
		return result ;
	}

	Lenth = strlen(TargetDevice);
	TempTxBuf[0] = 0x32;
	TempTxBuf[1] = 2+Lenth+2+DataLen;
	TempTxBuf[2] = (Lenth>>8)&0x00ff;
	TempTxBuf[3] = Lenth&0x00ff;

	memcpy(&TempTxBuf[4], (unsigned char*)TargetDevice, Lenth);
	TempTxBuf[4+Lenth] = 0x00;
	TempTxBuf[5+Lenth] = 0x01;

	memcpy(&TempTxBuf[6+Lenth], DataBuf, DataLen);
	Lenth = 2+2+Lenth+2+DataLen;

	result = rt_device_write(DevNew, 0, TempTxBuf, Lenth);

	return result;

}


/**
 *  AT client test
 * @param NULL
 * @return NULL    
 */
int  AT_Compoent_Init(void)
{   
    int Result = -1;
		
    Result =  at_client_init(AT_CLIENT_DEVICE,  512);//Set Receive Buf size is 512
	if(Result != RT_EOK)
	{
		rt_kprintf("Init AT Compoent Fail!\n");
	}

	return Result;

}


 /**
 * Send AT Order and Wait for ACK
 * @argc NULL
 * @argv NULL  
 */
 int ESP32_Close_ECHO(char* AT_CMD)
 {
	 at_response_t resp = RT_NULL;
	 int result = -1;
	 char Temp[10] ;

//Step1: Create and init  the resp sturct 	 
	 resp = at_create_resp(512, 0, 5);//Set Resp Receive Buf Capacity 512 , the respLie is 1~{#,~}waite for 5 sec
	 if(!resp)
	 {
		 rt_kprintf("create at resp fail !\n");
		 at_delete_resp(resp);
		 return RT_ERROR;
	 }
	 
//Step2:Send the ATCMD and waite for Resp	 
	 if(at_exec_cmd(resp, "ATE0") != RT_EOK)
	 {
		 rt_kprintf("send at ATE0 cmd fail !\n");
		 at_delete_resp(resp);
		 return RT_ERROR;
	 }
	 rt_kprintf("send at cmd ATE0 OK !\n");
	 
//Step3:parse the data received
	 result = at_resp_parse_line_args_by_kw(resp, "OK", "%s", Temp);

	 if(result >0 && memcmp((unsigned char*)Temp,"OK",2)==0 )
	 {
		 rt_kprintf("ESP32Close the ECHO =%s!\n", Temp);
	 }
	 else
	 {
		 rt_kprintf("ESP32CLose the ECHO FAIL~{#!~}");
	 }
	 
	  at_delete_resp(resp);
	  return RT_EOK;
 }



  /**
  * Send AT Order and Wait for ACK
  * @argc NULL
  * @argv NULL	
  */
  int ESP32_AT_Test(void)
  {
	  at_response_t resp = RT_NULL;
	  int result = -1;
	  char Temp[10] ;
 
 //Step1: Create and init  the resp sturct	  
	  resp = at_create_resp(128, 0, 5);//Set Resp Receive Buf Capacity 512 , the respLie is 1~{#,~}waite for 5 sec
	  if(!resp)
	  {
		  rt_kprintf("create at resp fail !\n");
			at_delete_resp(resp);
		  return RT_ERROR;
	  }
	  
 //Step2:Send the ATCMD and waite for Resp	  
	  if(at_exec_cmd(resp, "AT") != RT_EOK)
	  {
		  rt_kprintf("send at cmd fail !\n");
			at_delete_resp(resp);
		  return RT_ERROR;
	  }
	  rt_kprintf("send at cmd OK !\n");
	  
 //Step3:parse the data received
	  result = at_resp_parse_line_args_by_kw(resp, "OK", "%s", Temp);
 
	  if(result >0 && memcmp((unsigned char*)Temp,"OK",2)==0 )
	  {
		  rt_kprintf("ESP32 AT Test =%s!\n", Temp);
	  }
	  else
	  {
		  rt_kprintf("ESP32 AT Test Fail~{#!~}\n");
	  }
	  
	   at_delete_resp(resp);
	   return RT_EOK;
  }



   /**
   * Send the CMD and Wait for Resp .then Find the Key Word from the Resp Buf
   * @param TheATCMD without \r\n 
   * @param KeyWord  The keyWord to Check
   * @param WaitRespTime Wait Resp Time,if over the Time will Return RT_ERROR
   * @param ReceiveLineNum  Receive line Nums   0: no limit
   *
   * @return  0 Send AT CMD OK and  Recevie the KeyWord 
   * @return  Others  Sendthe CMD Fail
   */
   int ESP32_SendCOM_AT_CMD(char* AT_CMD, char*KeyWord, unsigned char WaitRespTime, unsigned char ReceiveLineNum)
   {
	   at_response_t resp = RT_NULL;
	   int result = -1;
	   char Temp[10] ;
  
  //Step1: Create and init	the resp sturct    
	   resp = at_create_resp(512, ReceiveLineNum, WaitRespTime);//Set Resp Receive Buf Capacity 512 , the respLie is 1~{#,~}waite for 5 sec
	   if(!resp)
	   {
		   rt_kprintf("create at resp fail !\n");
		   at_delete_resp(resp);
		   return RT_ERROR;
	   }
	   
  //Step2:Send the ATCMD and waite for Resp    
	   if(at_exec_cmd(resp, AT_CMD) != RT_EOK)
	   {
		   rt_kprintf("send at CMD = %s Fail!\n",AT_CMD);
		   at_delete_resp(resp);
		   return RT_ERROR;
	   }
	   rt_kprintf("send at CMD = %s OK!\n",AT_CMD);

	   rt_thread_delay(100);
	   
  //Step3:parse the data received
	   result = at_resp_parse_line_args_by_kw(resp, KeyWord, "%s", Temp);
  
	   if(result > 0 )
	   {
		   rt_kprintf(" at CMD = %s  resp parse OK!\n",AT_CMD);
	   }
	   else
	   {
		    rt_kprintf(" at CMD = %s  resp parse Fail!\n",AT_CMD);
		    at_delete_resp(resp);
		   	return -1;
	   }
	   
		at_delete_resp(resp);
		return RT_EOK;
   }

   /**
   * Startup the AT Thread to Receive the data AT server send 
   * @return  0 Startup The At Client Thread OK  
   * @return  Others  Fail
   */
   int StartUpATClientthread(void)
   {	
   		int Result = -1;
		
   		at_client_t  Temp_at_client = NULL;
		
	    Temp_at_client = at_client_get_first();
		if(Temp_at_client->parser != NULL)
		{
			Result = rt_thread_startup(Temp_at_client->parser);
			if(Result != RT_EOK)
			{
				rt_kprintf("Startup AT Client thread Fail \n");
			}
			else
			{
				rt_kprintf("Startup AT Client thread OK \n");
			}
			
		}

		return Result ;
	    
   }



   
   /**
   * Detach the AT Thread 
   * @return  0 Close The At Client Thread   
   * @return  Others  Close Fail
   */
   int DetachATClientthread(void)
   {	
   		int Result = -1;
		
   		at_client_t  Temp_at_client = NULL;
		
	    Temp_at_client = at_client_get_first();
		if(Temp_at_client->parser != NULL)
		{
			Result =  rt_thread_detach(Temp_at_client->parser);
			if(Result != RT_EOK)
			{
				rt_kprintf("Detach AT Client thread Fail \n");
			}
			else
			{
				rt_kprintf("Detach AT Client thread OK \n");
			}
			
		}

		return Result ;
	    
   }
	 
	 
	    /**
   * Detach the AT Thread 
   * @return  0 Close The At Client Thread   
   * @return  Others  Close Fail
   */
	void OneNet_RegisterDevice(void)
   {  
		 
	    int i=0;
   	  unsigned char TempBuf[256] = {0} ;

     rt_device_read(DevNew, 0, TempBuf, 256);
	   //sendHttpPkt(REG_PKT_HEAD, REG_PKT_BODY);
			
			rt_thread_delay(100);

	  rt_device_read(DevNew, 0, TempBuf, 256);

	  rt_kprintf("Register CMD Recive Data is = ");
	  
	  for(i=0 ; i<256; i++)
	  {
	  	 rt_kprintf("%c",TempBuf[i]);
	  }
   
	}


