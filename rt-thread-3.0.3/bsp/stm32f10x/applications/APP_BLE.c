
#include "APP_BLE.h"
#include "rtthread.h"
#include "Gsm_comm.h"
#include <at.h>


int BLE_Station = 0;


int APP_ESP32_Config_loop(void)
{   
	int i=0;
	int Result = -1;
	static int TryNum = 0;

    switch(BLE_Station)
    {
		case Reset:
		{       AT_COMPonet_Station = 1;
		        rt_thread_delay(100);
               rt_kprintf("ESP32_AT_BLE_Reset !\n");
				Result = ESP32_SendCOM_AT_CMD("AT+BLEINIT=0", "BLEINIT:0",300, 0);
			  BLE_Station =BLEINIT; 
		}

		case BLEINIT:
		{       			  
			Result = ESP32_SendCOM_AT_CMD(AT_BLEINIT, "BLEINIT:2",300, 0);
			if(Result == 0)
			{   
				TryNum = 0;
				BLE_Station = BLEGATTSSRV; //
				rt_kprintf("StepBLE1: Set AT+BLEINIT=2 !\n");
			}
			else
			{   
				Result = ESP32_SendCOM_AT_CMD("AT+BLEINIT=2", "OK",300, 0);
				if(Result == 0)
			    {   
					TryNum = 0;
					BLE_Station = BLEGATTSSRV; //
					rt_kprintf("StepBLE1: Set AT+BLEINIT=2 OK !\n");
			    }
				else
				{
					TryNum++;
				}
			    if(TryNum > MaxTryNum)
				{   
					TryNum = 0;
					BLE_Station =  Reset;
					rt_kprintf("StepBLE1: Set AT+BLEINIT=2  Fail !\n");			  
					return -1;//
			    }
		    }			
		
			break;
		}

		case BLEGATTSSRV:
		{       			  
			Result = ESP32_SendCOM_AT_CMD(AT_BLEGATTSSRV, "OK",300, 0);
			if(Result == 0)
			{   
				TryNum = 0;
				BLE_Station = BLEGATTSSRVSTART; //
				rt_kprintf("StepBLE: AT_BLEGATTSSRV  OK !\n");
			}
			else
			{   
				TryNum++;
			    if(TryNum > MaxTryNum)
				{   
					TryNum = 0;
					BLE_Station =  Reset;		  
					return -1;//
			    }
		    }			
	
			break;
		}
		
		case BLEGATTSSRVSTART:
		{       			  
			Result = ESP32_SendCOM_AT_CMD(AT_BLEGATTSSRVSTART, "OK",300, 0);
			if(Result == 0)
			{   
				TryNum = 0;
				BLE_Station = BLEADDR; //
				rt_kprintf("StepBLE3: AT_BLEGATTSSRV  OK !\n");
			}
			else
			{   
				TryNum++;
			    if(TryNum > MaxTryNum)
				{   
					TryNum = 0;
					BLE_Station =  Reset;		  
					return -1;//
			    }
		    }			

			break;
		}


		case BLEADDR :
		{       			  
			Result = ESP32_SendCOM_AT_CMD(AT_BLEADDR, "OK",300, 0);
			if(Result == 0)
			{   
				TryNum = 0;
				BLE_Station = BLEADVPARAM; //
				rt_kprintf("StepBLE4: AT_BLEGATTSSRV  OK !\n");
			}
			else
			{   
				TryNum++;
			    if(TryNum > MaxTryNum)
				{   
					TryNum = 0;
					BLE_Station =  Reset;		  
					return -1;//
			    }
		    }			

			break;
		}

		case BLEADVPARAM:
		{       			  
			Result = ESP32_SendCOM_AT_CMD(AT_BLEADVPARAM, "OK",300, 0);
			if(Result == 0)
			{   
				TryNum = 0;
				BLE_Station = BLEADVDATA; //
				rt_kprintf("StepBLE5: AT_BLEGATTSSRV  OK !\n");
			}
			else
			{   
				TryNum++;
			    if(TryNum > MaxTryNum)
				{   
					TryNum = 0;
					BLE_Station =  Reset;		  
					return -1;//
			    }
		    }			

			break;
		}

		case BLEADVDATA:
		{       			  
			Result = ESP32_SendCOM_AT_CMD(AT_BLEADVDATA, "OK",300, 0);
			if(Result == 0)
			{   
				TryNum = 0;
				BLE_Station = BLEADVSTART; //
				rt_kprintf("StepBLE6: AT_BLEGATTSSRV  OK !\n");
			}
			else
			{   
				TryNum++;
			    if(TryNum > MaxTryNum)
				{   
					TryNum = 0;
					BLE_Station =  Reset;		  
					return -1;//
			    }
		    }			

			break;
		}

		case BLEADVSTART:
		{       			  
			Result = ESP32_SendCOM_AT_CMD(AT_BLEADVSTART, "OK",300, 0);
			if(Result == 0)
			{   
				TryNum = 0;
				BLE_Station = BLEOK; //
				AT_COMPonet_Station = 0;
				rt_thread_delay(100);
				rt_kprintf("StepBLE7: AT_BLEGATTSSRV  OK !\n");
			}
			else
			{   
				TryNum++;
			    if(TryNum > MaxTryNum)
				{   
					TryNum = 0;
					BLE_Station =  Reset;		  
					return -1;//
			    }
		    }			

			break;
		}

    }

}


