


stESP32_BLE_AT  ESP32_BLE_AT[20]  = { 
	                                  {"AT+BLEINIT=2","OK",100},
	                                  {"AT+BLEGATTSSRVCRE","OK",100},
									  {"AT+BLEGATTSSRVSTART","OK",100},
									  {"AT+BLEADDR?","OK",100},
									  {"AT+BLEADVPARAM=32,64,0,0,7,"OK",100},
									  {"AT+BLEADVDATA=¡°0201060A09457370726573736966030302A0¡±","OK",100},
									  {"AT+BLEADVSTART","OK",100},
														  	
                                     };

                                     

void APP_ESP32_BLE_Config(void)
{    

	for(i=0;i<10;)
	{

	   	Result = ESP32_SendCOM_AT_CMD(ESP32_BLE_AT[i].AT_CMD, ESP32_BLE_AT[i].AT_CMD_RESP_KEY,ESP32_BLE_AT[i].AT_CMD_Wait_Time, 0);
		if(Result == 0)
		{   
			continue;
			i++
			rt_kprintf("Step%d:%s Set OK !\n",i,ESP32_BLE_AT[i].AT_CMD);
		}
		else
		{	  
			i=0;
		}
	}

				
}


