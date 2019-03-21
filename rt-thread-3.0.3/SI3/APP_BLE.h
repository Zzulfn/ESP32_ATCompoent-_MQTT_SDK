#ifndef _APP_BLE_H
#define _APP_BLE_H

#define BLEINIT           0
#define BLEGATTSSRV       1
#define BLEGATTSSRVSTART  2
#define	BLEADDR           3
#define	BLEADVPARAM       4
#define	BLEADVDATA        5
#define	BLEADVSTART       6


#define AT_BLEINIT           "AT+BLEINIT?"
#define AT_BLEGATTSSRV       "AT+BLEGATTSSRV"
#define AT_BLEGATTSSRVSTART  "AT+BLEGATTSSRVSTART"
#define AT_BLEADDR           "AT+BLEADDR?"
#define AT_BLEADVPARAM       "AT+BLEADVPARAM=32,64,0,0,7"
#define AT_BLEADVDATA        "AT+BLEADVDATA=\"0201060A09457370726573736966030302A0\""
#define AT_BLEADVSTART       "AT+BLEADVSTART"


typedef struct ESP32_BLE_AT
{
	char AT_CMD[100];
	char AT_CMD_RESP_KEY[30];
	int  AT_CMD_Wait_Time;
}stESP32_BLE_AT;

extern stESP32_BLE_AT  ESP32_BLE_AT[10];

extern void APP_ESP32_BLE_Config(void);




#endif
