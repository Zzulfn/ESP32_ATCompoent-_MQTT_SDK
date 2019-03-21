#ifndef _APP_ONENET_H
#define _APP_ONENET_H
#include "rtthread.h"
#include "mqtt.h"

/*****************OneNetDefauolParam**************************/

#define PROD_ID_Default   "196724" 

//#define PROD_KEY_Default  ""

#define SN        "808"     
#define REG_CODE_Default  "M4H1KwHiGI0wjZ1U"  
#define API_ADDR_Default  "api.heclouds.com"

#define Http_IPAddr_Default "183.230.40.33"
#define Http_ServerPort   80

//#define Http_IPAddr_Default "112.74.59.250"//ͨѶè��ַ
//#define Http_ServerPort   10002

#define MQTT_IPAddr_Default "183.230.40.39"
#define MQTT_ServerPort   6002


#define DEVICE_NAME     "AMC_"SN
#define REG_PKT_HEAD    "POST http://"API_ADDR"/register_de?register_code="REG_CODE" HTTP/1.1\r\n" "Host: "API_ADDR"\r\n" "Content-Length: "                
#define REG_PKT_BODY    "{\"title\":\""DEVICE_NAME"\",\"sn\":\""SN"\"}\r\n"

#define WIFI_Name_Default   "HMDJ"
#define WIFI_KEY_Default    "88888888"


#define DeviceRegist_OK   1
#define DeviceReGist_FAIL 0

typedef struct _ONENETPARAM
{   
	rt_mutex_t  OneNetParamLock;// OneNet Param lock
	char*API_ADDR;// Http Server Addr 
	char*PROD_ID;  //product ID 
	char*REG_CODE;//Product register Code 
	char*SN_Num; //Product Device SN

	char*DeviceNames; //Device Name 
	char Device_ID[10];//Device ID
	char Device_Key[30];
	
	char HTTP_Addr[30];//Http Server IP Addr 183.230.40.33
	int  HTTP_port;//80 
	char DeviceResgistMark;//1 Regist OK

	char GW_Name[30];
	char GW_KEY[20];
	
	char MQTT_ADDR[30];//MQTT Server IP Addr 183.230.40.39
	int MQTT_Port;//6002
	mqtt_client_t *mqtt_client;// mqtt client 
	struct mqtt_connect_client_info_t Mqtt_Connect_info;
	
}stOneNetParam,*pOneNetParam;
extern stOneNetParam OneNetParam;


extern int  OneNet_ChangeServer(char*ServeIP, int ServerPort);
extern int  RegisterDevice(stOneNetParam*ProOneNetParam);
extern int  OneNetParaInit(void);
extern  int OneNet_Connect(void);
extern int OneNet_SUBSCRIBE(char*Sub_Topic);

extern rt_size_t rt_ESP32_write(rt_device_t dev,
                          rt_off_t    pos,
                          const void *buffer,
                          rt_size_t   size);



#endif
