#include "GPT.h"
//#include "ApplicationLogic.h"


extern void CallbackFunc(void);




const Gpt_ConfigType GptConfigArr=
{
	{GPT_TIMER_A_0_16_32_BIT,16,50,GPT_PRIODIC_TIMER_MODE,CallbackFunc}
};