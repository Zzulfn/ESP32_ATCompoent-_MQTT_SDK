
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
	unsigned int  ZeroNum = 10000; //�����жϵ�λ�Ƿ�ȫΪ0ȫΪ0 �ǲ���Ҫ������ ���� 5000 ֻ����5ǧ

//Step1:�жϴ����������Ƿ�Ϊ��,����ǧ���µ�����һ�����ϵ�ֻ���ŵ�ǧλ
	Num = Num %10000;
	if(Num ==0)
	{
	    return ;
	}
	
//Step2:�ж�ǧλ�Ƿ�Ϊ0
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
					if(Num%ZeroNum != 0 && Temp[j+1] != 0)//���ǰ��Ϊ��Ϊ0��ǰλΪ0����Ҫ����0
					{
						Voice_Play(0); //����ȫΪ0, ��������������Դ˴������0
					}
                  
                      
				}

               
        if(Num%ZeroNum == 0) //�������ȫΪ0���򷵻�
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
* @brief   ��������������  
* @retval  NULL
*/
void VoicePlayHandle(void)
{   
    unsigned int Num[4] = {0};
    unsigned int Money = 0;
    stUserPayParam *pVoicePlayData ;
   

    if(VoicePlay.HeardIndex == VoicePlay.TailIndex)
    {
        return ; //��ͷ����βʱ����Ҫ����
    }

 //��ȡ��Ҫ���������ݵĵ�ַ
    pVoicePlayData = &VoicePlay.PalyParam[VoicePlay.HeardIndex];
    VoicePlay.HeardIndex++;
    if(VoicePlay.HeardIndex == MAXVOICENUM)
    {
        VoicePlay.HeardIndex = 0;
    }
    
    Money =  pVoicePlayData->Money;
        
    Num[0] = Money%10;    //��
    Num[1] = Money%100;   //��
    Num[2] = (Money/100)%10000;//0��ǧ
    Num[3] = Money/1000000;//����


 //���������϶�
     VoicePlayerLowNum(Num[3]);
     if(Num[3]%10000 !=0 )
     {
     
     	 Voice_Play(Voice_10000);//��
     }


    
 //����0-9999��
     VoicePlayerLowNum(Num[2]);
     if(Num[0]==0 && Num[1]==0)
     {
       Voice_Play(Voice_yuan);//Ԫ
       return;//����û�����֣��򵽴˲������
     }
     else
     {
       Voice_Play(Voice_dian);//��
     }


     

//����ֽǶ�
     VoicePlayerLowNum(Num[1]); 

     if(Num[0] !=0)
     {
        VoicePlayerLowNum(Num[0]);
     }

     Voice_Play(Voice_yuan);//Ԫ
 
 
     

}







