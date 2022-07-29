 /******************************************************************************
 *
 * Module: GPT
 *
 * File Name: GPT_PBCfg.c
 *
 * Author: Youssef hussien
 ******************************************************************************/

#include "GPT.h"



extern void CallbackFunc(void);




const Gpt_ConfigType GptConfigArr=
{
	{GPT_TIMER_A_0_16_32_BIT,200,GPT_PRIODIC_TIMER_MODE,CallbackFunc}
};