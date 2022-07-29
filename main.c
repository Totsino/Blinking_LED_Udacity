#include "Std_Types.h"
#include "Led.h"
#include "tm4c123gh6pm_registers.h"
#include "gpio.h"
#include "Port.h"
#include "GPT.h"
#include "NVIC.h"

/* Configrations of User defined time */

#define APPLICATION_LOGIC_CFG_ON_TIME			5 		//in seconds
#define	APPLICATION_LOGIC_CFG_OFF_TIME		2			//in seconds


void Clock_init(void);

void Clock_init(void){
	
	/* Activate all Ports Clock */
  SYSCTL_REGCGC2_REG |=(0x3F);
	
	
	/* Delay 3 System Clock cycles before accessing */
  volatile unsigned long delay = 0;
  delay = SYSCTL_REGCGC2_REG;

}


void AppDriversInit(void);
void CallbackFunc(void);
static uint8 LocSeconds;
static boolean LedState=FALSE;

int main(){
	
	AppDriversInit();
while(1){

	

	
	if(LedState)
	{

		if(LocSeconds>=APPLICATION_LOGIC_CFG_ON_TIME)
		{
			LED_setOff();
			LocSeconds=0;
			LedState=FALSE;
		}
	}
	else
	{
	
		if(LocSeconds>=APPLICATION_LOGIC_CFG_OFF_TIME)
		{
			LED_setOn();
			LocSeconds=0;
			LedState=TRUE;
		}
	}

}


}




void AppDriversInit(void)
{
	Clock_init();
	Port_Init(&Port_Configuration);
	Dio_Init(&Dio_Configuration);
	Nvic_Init(&NVIC_Configrations);
	Gpt_Init(&GptConfigArr);
	Gpt_EnableNotification(GPT_TIMER_A_0_16_32_BIT);
	Gpt_StartTimer(GPT_TIMER_A_0_16_32_BIT,62500);
	
}



void CallbackFunc(void)
{
   LocSeconds++;
}








