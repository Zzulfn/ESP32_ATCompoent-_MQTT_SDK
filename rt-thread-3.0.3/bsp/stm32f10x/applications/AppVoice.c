
#include "AppVoice.h"
#include "Gsm_comm.h"
#include <rtthread.h>
void VoicePlayHandle(void);
extern stVoicePaly VoicePlay;


/*********************************************************************************************************
* @brief   Voice Thread handle
* @retval   -1 Fail   0 connected
*/

void Voice_thread_entry(void)
{ 
  rt_thread_delay(50);
  Voice_Init();
  rt_kprintf("Voice thread start!\n");
  
  while(1)
  {
    VoicePlayHandle();
    rt_thread_delay(50);
  }
  
}



/*********************************************************************************************************
* @brief   void VoicePlayerLowNum(unsigned int Num)
* @retval   -1 Fail   0 connected
*/
void VoicePlayerLowNum(unsigned int Num)
{ 
	unsigned int  Temp[4];
	 int  i = 0;
	unsigned char j=0;
	unsigned int  ZeroNum = 10000; //用于判断低位是否全为0全为0 是不需要播报的 比如 5000 只播报5千

//Step1:判断传进来的数是否为零,播报千以下的数字一万以上的只播放到千位
	Num = Num %10000;
	if(Num ==0)
	{
	    return ;
	}
	
//Step2:判断千位是否为0
    Temp[3] = Num/1000; 
    Temp[2] = (Num%1000)/100;
    Temp[1] = (Num%100)/10;
    Temp[0] = Num%10;

	 for(i=3; i>=0; i--)
	 {  
	 	ZeroNum = ZeroNum /10;

        if(Temp[i] != 0)
        {
			for(j= i; j>=0; j--)
			{   
				
				if(Temp[j] != 0)
				{  
				
				   Voice_Play(Temp[j]);
                   if(j-1>=0)
                   {
                     Voice_Play(Voice_10+j-1);//
                   }

                   
                   
				}
				else
				{   
					if(Num%ZeroNum != 0 && Temp[j+1] != 0)//如果前面为不为0当前位为0，怎要播报0
					{
						Voice_Play(0); //后面全为0, 不会流到这里，所以此处必须读0
					}
                  
                      
				}

               
        if(Num%ZeroNum == 0) //如果后面全为0，则返回
        {
            return;
        }
                   
				 ZeroNum = ZeroNum /10;
				
                
			}


        }
	   
		
	 }
	  return;


}



/*********************************************************************************************************
* @brief   语音播报处理函数  
* @retval  NULL
*/
void VoicePlayHandle(void)
{   
    unsigned int Num[4] = {0};
    unsigned int Money = 0;
    stUserPayParam *pVoicePlayData ;
   

    if(VoicePlay.HeardIndex == VoicePlay.TailIndex)
    {
        return ; //当头等于尾时不需要播报
    }

 //获取将要播报的数据的地址
    pVoicePlayData = &VoicePlay.PalyParam[VoicePlay.HeardIndex];
    VoicePlay.HeardIndex++;
    if(VoicePlay.HeardIndex == MAXVOICENUM)
    {
        VoicePlay.HeardIndex = 0;
    }
    
    Money =  pVoicePlayData->Money;
        
    Num[0] = Money%10;    //分
    Num[1] = Money%100;   //角
    Num[2] = (Money/100)%10000;//0到千
    Num[3] = Money/1000000;//几万


 //播报万及以上段
     VoicePlayerLowNum(Num[3]);
     if(Num[3]%10000 !=0 )
     {
     
     	 Voice_Play(Voice_10000);//万
     }


    
 //播报0-9999段
     VoicePlayerLowNum(Num[2]);
     if(Num[0]==0 && Num[1]==0)
     {
       Voice_Play(Voice_yuan);//元
       return;//后面没有数字，则到此播报完毕
     }
     else
     {
       Voice_Play(Voice_dian);//点
     }


     

//处理分角段
     VoicePlayerLowNum(Num[1]); 

     if(Num[0] !=0)
     {
        VoicePlayerLowNum(Num[0]);
     }

     Voice_Play(Voice_yuan);//元
 
 
     

}







