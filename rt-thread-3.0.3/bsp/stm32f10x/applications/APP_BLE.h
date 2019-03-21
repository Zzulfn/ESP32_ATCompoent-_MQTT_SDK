#ifndef _APP_BLE_H
#define _APP_BLE_H


#define Reset             0
#define BLEINIT           1
#define BLEGATTSSRV       2
#define BLEGATTSSRVSTART  3
#define	BLEADDR           4
#define	BLEADVPARAM       5
#define	BLEADVDATA        6
#define	BLEADVSTART       7
#define BLEOK             8


#define AT_BLEINIT           "AT+BLEINIT?"
#define AT_BLEGATTSSRV       "AT+BLEGATTSSRVCRE"
#define AT_BLEGATTSSRVSTART  "AT+BLEGATTSSRVSTART"
#define AT_BLEADDR           "AT+BLEADDR?"
#define AT_BLEADVPARAM       "AT+BLEADVPARAM=32,64,0,0,7"
#define AT_BLEADVDATA        "AT+BLEADVDATA=\"0201060A09457370726573736966030302A0\""
#define AT_BLEADVSTART       "AT+BLEADVSTART"

extern int BLE_Station ;


extern void APP_ESP32_BLE_Config(void);
extern int APP_ESP32_Config_loop(void);




#endif
