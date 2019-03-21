
#include "APP_OneNet.h"
#include "stdio.h"
#include <rtthread.h>
#include "board.h"
#include "Gsm_comm.h"
#include "mqtt.h"
#include "at.h"

stOneNetParam OneNetParam;

/**
* Send the Http pkt , register device and get the device ID 
* @param   phead 
* @param   pbody
* @return  NULL
*/

rt_size_t rt_ESP32_write(rt_device_t dev,
                          rt_off_t    pos,
                          const void *buffer,
                          rt_size_t   size)
{       
	 char Temp[20] = {0};
	 int length = -1;
	
	 sprintf(Temp,"AT+CIPSEND=%d\r\n",size);
	 rt_device_write(DevNew, 0, Temp, 15);// Send the data to Server 
	 rt_thread_delay(100);
	 length = rt_device_write(DevNew, 0, buffer, size);// Send the data to Server 
	 return length;
 }



/**
* Send the Http pkt , register device and get the device ID 
* @param   phead 
* @param   pbody
* @return  NULL
*/
int  RegisterDevice(stOneNetParam*ProOneNetParam)
{
    char sendBuf2[100]={0};
	char sendBuf1[400]={0};
	int i=0;
	int Lenth =0;
	va_list args;
	int resp_args_num = 0;
	char temp =0;
    rt_mutex_take(OneNetParam.OneNetParamLock, RT_WAITING_FOREVER);
	
	rt_device_read(DevNew, 0, sendBuf2, sizeof(sendBuf2));
    
	memset((unsigned char *)&sendBuf2, 0x00, sizeof(sendBuf2));
	sprintf(sendBuf1, "POST http://%s/register_de?register_code=%s HTTP/1.1\r\n",OneNetParam.API_ADDR, OneNetParam.REG_CODE);
	rt_kprintf("Len= %d:::%s",strlen(sendBuf1),sendBuf1);
	rt_ESP32_write(DevNew, 0, sendBuf1, strlen(sendBuf1));// Send the data to Server 
	//rt_device_write(DevNew, 0, sendBuf1, strlen(sendBuf1));// Send the data to Server 
	rt_thread_delay(100);
  Lenth = rt_device_read(DevNew, 0, sendBuf1, sizeof(sendBuf1));
  for(i=0;i<Lenth;i++ )
	{
		rt_kprintf("%c",sendBuf1[i]);
	}
	memset((unsigned char*)&sendBuf1, 0x00, sizeof(sendBuf1) );
	
	sprintf(sendBuf1, "Host: %s\r\n", OneNetParam.API_ADDR);
	rt_kprintf("Len= %d:::%s",strlen(sendBuf1),sendBuf1);
	rt_ESP32_write(DevNew, 0, sendBuf1, strlen(sendBuf1));// Send the data to Server 
	//rt_device_write(DevNew, 0, sendBuf1, strlen(sendBuf1));// Send the data to Server 
	rt_thread_delay(100);
    Lenth = rt_device_read(DevNew, 0, sendBuf1, sizeof(sendBuf1));
	for(i=0;i<Lenth;i++ )
	{
		rt_kprintf("%c",sendBuf1[i]);
	}
	memset((unsigned char*)&sendBuf1, 0x00, sizeof(sendBuf1) );
	sprintf(sendBuf2, "{\"title\":\"%s\",\"sn\":\"%s\"}",OneNetParam.DeviceNames,OneNetParam.SN_Num);
	sprintf(sendBuf1, "Content-Length:%d\r\n\r\n",strlen(sendBuf2));
	
    for(i=0;i<strlen(sendBuf2);i++ )
	{
		rt_kprintf("%c",sendBuf2[i]);
	}

	rt_kprintf("Len= %d:::%s",strlen(sendBuf1),sendBuf1);
	rt_ESP32_write(DevNew, 0, sendBuf1, strlen(sendBuf1));// Send the data to Server 
	//rt_device_write(DevNew, 0, sendBuf1, strlen(sendBuf1));// Send the data to Server 
	rt_thread_delay(100);
     Lenth =  rt_device_read(DevNew, 0, sendBuf1, sizeof(sendBuf1));

    for(i=0;i<Lenth;i++ )
	{
		rt_kprintf("%c",sendBuf1[i]);
	}
    memset((unsigned char*)&sendBuf1, 0x00, sizeof(sendBuf1) );
	
	rt_kprintf("%s",sendBuf2);
	rt_ESP32_write(DevNew, 0, sendBuf2, strlen(sendBuf2));// Send the data to Server 
	//rt_device_write(DevNew, 0, sendBuf2, strlen(sendBuf2));// Send the data to Server 
	
     memset((unsigned char*)&sendBuf1, 0x00, sizeof(sendBuf1) );
	rt_thread_delay(1000);
	Lenth = rt_device_read(DevNew, 0, sendBuf1, sizeof(sendBuf1));

	for(i=0;i<Lenth;i++ )
	{
		rt_kprintf("%c",sendBuf1[i]);
	}

 	if(Lenth > 0)
    {
        int p = 0, pend = 0;
        /* get the deivce id  */
        if((p=strstr(sendBuf1, "device_id")) != NULL)
        {   
            p += strlen("device_id\":\"");
            if((pend=strstr(p, "\",")) != NULL)
            {   
				temp = pend-p;
				if(temp<=10)
				{
					memcpy(OneNetParam.Device_ID, p, pend-p);
					rt_kprintf("*********OneNetPram.DeviceID = %s*************\n",OneNetParam.Device_ID);
					rt_mutex_release(OneNetParam.OneNetParamLock);
					return 0;//
				}
											
            }
        }
    }
    rt_mutex_release(OneNetParam.OneNetParamLock);

	return -1;

}


/**
* Change  the connect Server 
* @param   ServerIP    ServerIP 
* @param   ServverPort Serverport 
* @return  0 OK， others Fail
*/
int OneNet_ChangeServer(char*ServeIP, int ServerPort)
{
	int result = -1;
	char TempAT_CMD[100] = {0};
	int TryNum = 3;
	
//Step1: leave the pass through station 
	if(DevNew != NULL)
	{
		rt_kprintf("OneNet_ChangeServer DevNew == NULL!!");
		return -1;
	}
	
	rt_device_write(DevNew, 0, AT_LeaveTranMode, strlen(AT_LeaveTranMode));	
  AT_COMPonet_Station = 1;//Enable the AT Receive
	rt_thread_delay(500);//must wait 1s at least before send next AT CMD
	
//Step2: disconnect the Server 	
	result = ESP32_SendCOM_AT_CMD(AT_CIPCLOSE, "OK", 100, 0);
	if(result != 0)
	{
		rt_kprintf("disconnnet server fail !\n");
		return -1;
	}

//Step2: connect to the New server 
	sprintf(TempAT_CMD,"AT+CIPSTART=\"TCP\",\"%s\",%d",ServeIP, ServerPort);
	result = ESP32_SendCOM_AT_CMD(TempAT_CMD, "OK", 100, 0);
	if(result != 0)
	{
		rt_kprintf("Connect to%s:%d Fail \n",ServeIP,ServerPort);
		return -1;
	}
    rt_kprintf("Connect to%s:%d  \n",ServeIP,ServerPort);
	
//Step3: enter the pass through station  
	result = ESP32_SendCOM_AT_CMD(AT_SetTranMode, "OK", 500, 0);
	if(result != 0)
	{
		rt_kprintf("Enter ther Pass through Station Fail !\n");
		return -1;
	}
	rt_kprintf("Enter ther Pass through Station");
	
	result = ESP32_SendCOM_AT_CMD(AT_StartSendData, "OK", 100, 0);
	if(result != 0)
	{
		rt_kprintf("Stat to send Data Fail \n ");
		return -1;
	}

	rt_kprintf("Now Stat to send Data !!!! \n ");

	return RT_EOK;

}



/**
* init the OneNetParam  struct 
* @param   NULL
* @return  
*/ 
int OneNetParaInit(void)
{  
	int result = -1;
	
	OneNetParam.OneNetParamLock = rt_mutex_create("OneNetLOCK", 1);//
	if(OneNetParam.OneNetParamLock == NULL)
	{
		rt_kprintf(" Create OneNet LOCK Fail !\n");
		return  result ; 
	}

	OneNetParam.API_ADDR =  API_ADDR_Default;
	memcpy(OneNetParam.HTTP_Addr, Http_IPAddr_Default, strlen(Http_IPAddr_Default));
	OneNetParam.HTTP_port = Http_ServerPort;
	
	OneNetParam.PROD_ID =  PROD_ID_Default;
	OneNetParam.REG_CODE = REG_CODE_Default;

	OneNetParam.SN_Num= SN;
	OneNetParam.DeviceNames = DEVICE_NAME;
	

    memcpy(OneNetParam.MQTT_ADDR, Http_IPAddr_Default, strlen(Http_IPAddr_Default));
	OneNetParam.MQTT_Port = Http_ServerPort;
	OneNetParam.mqtt_client = mqtt_client_new();
	if(OneNetParam.mqtt_client == RT_NULL)
	{
		rt_kprintf("OneNet Mqtt Client New  Fail !!!\n");
		return RT_ERROR;
	}

	memcpy((unsigned char*)&OneNetParam.GW_Name,WIFI_Name_Default, strlen(WIFI_Name_Default));
	memcpy((unsigned char*)&OneNetParam.GW_KEY,WIFI_KEY_Default, strlen(WIFI_KEY_Default));


	OneNetParam.Mqtt_Connect_info.client_id = OneNetParam.Device_ID;
	OneNetParam.Mqtt_Connect_info.client_pass = OneNetParam.SN_Num;
	OneNetParam.Mqtt_Connect_info.client_user = OneNetParam.PROD_ID;
	OneNetParam.Mqtt_Connect_info.keep_alive = 120;//两分钟；
	OneNetParam.Mqtt_Connect_info.will_msg = NULL;
	OneNetParam.Mqtt_Connect_info.will_qos = NULL;
	OneNetParam.Mqtt_Connect_info.will_topic = NULL;



	return RT_EOK;
	
}

/**
* init the OneNetParam  struct 
* @param   NULL
* @return  0 OK， others Fail
*/ 
int OneNet_Connect(void)
{   
    unsigned  int lenth =0;

	struct mqtt_connect_client_info_t client_info;

	int result = -1;
    if(OneNetParam.mqtt_client ==NULL)
    {
		rt_kprintf("OneNet Connect Fail as Mqtt_Client ==NULL !!!!!\n");
		return result;
		
    }
    OneNetParam.mqtt_client->conn_state = 0;

//	  OneNetParam.mqtt_client->conn =1;
	

	lenth = rt_device_read(DevNew, 0, OneNetParam.mqtt_client->rx_buffer, MQTT_VAR_HEADER_BUFFER_LEN);
		result =  mqtt_client_connect(OneNetParam.mqtt_client,  NULL, 0, NULL, NULL,
                   &OneNetParam.Mqtt_Connect_info);

	rt_thread_delay(500);
    result = -1;
	lenth = rt_device_read(DevNew, 0, OneNetParam.mqtt_client->rx_buffer, MQTT_VAR_HEADER_BUFFER_LEN);
	if(lenth > 0)
	{   
	    
		unsigned int i=0;
		rt_kprintf("接收数据=");
		for(i=0; i<lenth;i++)
		rt_kprintf("0x%x", OneNetParam.mqtt_client->rx_buffer[i]);

		rt_kprintf("\n");
		result = mqtt_message_received(OneNetParam.mqtt_client, 2, lenth,0);
	}

	if(result == MQTT_CONNECT_ACCEPTED)
	{
		return RT_EOK;
	}
	
	return -1;
					
}




/**
* init the OneNetParam  struct 
* @param   NULL
* @return  0 OK， others Fail
*/ 
int OneNet_SUBSCRIBE(char*Sub_Topic)
{   
    unsigned  int lenth =0;

	struct mqtt_connect_client_info_t client_info;

	int result = -1;
    if(OneNetParam.mqtt_client ==NULL)
    {
		rt_kprintf("OneNet Connect Fail as Mqtt_Client ==NULL !!!!!\n");
		return result;
		
    }
//    OneNetParam.mqtt_client->conn_state = 0;

//	OneNetParam.mqtt_client->conn =1;
	lenth = rt_device_read(DevNew, 0, OneNetParam.mqtt_client->rx_buffer, MQTT_VAR_HEADER_BUFFER_LEN);

	result =mqtt_sub_unsub(OneNetParam.mqtt_client, Sub_Topic, 0, NULL, NULL, 1);
		result = -1;
		
	rt_thread_delay(500);

	lenth = rt_device_read(DevNew, 0, OneNetParam.mqtt_client->rx_buffer, MQTT_VAR_HEADER_BUFFER_LEN);
	if(lenth > 0)
	{    
			    
		unsigned int i=0;
		rt_kprintf("SUB =");
		for(i=0; i<lenth; i++)
		rt_kprintf("0x%x", OneNetParam.mqtt_client->rx_buffer[i]);

		rt_kprintf("\n");
		result = mqtt_message_received(OneNetParam.mqtt_client, 2, lenth,0);
	}

	if(result == MQTT_CONNECT_ACCEPTED)
	{
		return RT_EOK;
	}
	
	return -1;
					
}



void OneNet_Connect_CallBack (mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
	rt_kprintf("receive connect ack");
}


int FindStr( char *SourceData,int SourceLen, char*DesData, int DesLen)
{
	int Index = 0;
	int result = -1;
	
	if(SourceData == NULL ||  DesData==NULL )
	{
		rt_kprintf("FindStr NULL Fail!!");
		return result ;
	}

	if( DesLen > SourceLen)
	{
		rt_kprintf("DesData > DesLen");
		return result;
	}

	for(Index; Index < (SourceData-))
}


/****************FILE END********************/




