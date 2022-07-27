/**********************************************************************************************************************
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/**        \file  GPT.c
 *        \brief  General Purpose Timers Driver
 *
 *      \details  A driver used to configure the Timer peripheral and set its modes
 *
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "GPT.h"

/**********************************************************************************************************************
*  LOCAL MACROS CONSTANT\FUNCTION
*********************************************************************************************************************/
#define BIT0	0
#define BIT1	1
#define BIT2	2
#define BIT3	3
#define BIT4	4
#define BIT5	5
#define BIT6	6
#define BIT7	7
#define BIT8	8
#define BIT9	9
#define BIT10	10
#define BIT11	11
#define BIT12	12
#define BIT13	13
#define BIT14	14
#define BIT15	15
#define BIT16	16
#define TIMERA 0
#define TIMERB 1
#define TIMER16 0
#define TIMER32 1

#define RCGC_BITS 6

#define GPT_START_COUNT_ENABLE 	1
#define TIMER_COUNT							12
/**********************************************************************************************************************
 *  LOCAL DATA 
 *********************************************************************************************************************/
static const uint32 GPT_TimerChannelBaseAddress[GPT_MAX_CHANNELS]=
{
	GPT_TIMER0_BASE_ADDRESS,
	GPT_TIMER1_BASE_ADDRESS,
	GPT_TIMER2_BASE_ADDRESS,
	GPT_TIMER3_BASE_ADDRESS,
	GPT_TIMER4_BASE_ADDRESS,
	GPT_TIMER5_BASE_ADDRESS,
	GPT_WIDE_TIMER0_BASE_ADDRESS,
	GPT_WIDE_TIMER1_BASE_ADDRESS,
	GPT_WIDE_TIMER2_BASE_ADDRESS,
	GPT_WIDE_TIMER3_BASE_ADDRESS,
	GPT_WIDE_TIMER4_BASE_ADDRESS,
	GPT_WIDE_TIMER5_BASE_ADDRESS,
};



uint8 					LocCounter=0;
Gpt_ChannelType LocChannelId;
Gpt_Frequency		LocFrequency;
Gpt_ValueType 	LocTicks		;
Gpt_Mode				LocMode			;
uint32					LocBaseAdd	;
uint8   				LocTimerAB	;
uint8						LocTimerType;

/**********************************************************************************************************************
 *  GLOBAL DATA
 *********************************************************************************************************************/
static const Gpt_ConfigType *  global_Config;
static void (*GptNotification[GPT_MAX_CHANNELS]) (void);
static const Gpt_ConfigChannel * Channel;
/**********************************************************************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  LOCAL FUNCTIONS
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 *********************************************************************************************************************/


/******************************************************************************
* \Syntax          : void Gpt_Init(const Gpt_ConfigType* ConfigPtr)        
* \Description     : Initializes the hardware timer module.                                   
*                                                                             
* \Sync\Async      : Synchronous                                               
* \Reentrancy      : Non Reentrant                                             
* \Parameters (in) : ConfigPtr - Pointer to a selected configuration structure                    
* \Parameters (out): None                                                      
* \Return value:   : None                               
*******************************************************************************/
void Gpt_Init(const Gpt_ConfigType* ConfigPtr)  
{
	global_Config=ConfigPtr;
	if(global_Config!=NULL_PTR)
	{
		
		for(;LocCounter<GPT_CFG_CONFIGURED_CHANNELS;LocCounter++)
		{
			Channel=ConfigPtr[LocCounter].channels;
			
			LocChannelId								=Channel->GptChannelId										;
			LocFrequency								=Channel->GptChannelTickFrequency					;
			LocMode											=Channel->GptChannelMode									;
			LocTicks										=Channel->GptChannelTickValueMax					;
			LocBaseAdd									=GPT_TimerChannelBaseAddress[LocChannelId%12];
			GptNotification[LocChannelId%12]	=Channel->GptNotifications								;
			/*Configuration parameters now are saved into the local variables and the base address of the timer is saved*/
					
		if(LocChannelId/TIMER_COUNT)
		{
			LocTimerAB=TIMERB;
			LocChannelId-=TIMER_COUNT;
		}
		else
		{
			LocTimerAB=TIMERA;
			
		}
		/*Timer channel is now determined A or B*/
		if(LocChannelId>5 && LocChannelId<12)
		{
			LocTimerType=TIMER32;
		}
		else
		{
			LocTimerType=TIMER16;
		}
		/*Timer type is now determined wide or normal*/
		if(LocTimerType==TIMER16)
		{
			SET_BIT(SYSCTL_RCGC_REG,LocChannelId);
		}
		else
		{
			uint8 TempChannelID = LocChannelId-6;
			SET_BIT(SYSCTL_RCGCW_REG,TempChannelID);
		}
		/*Enabling RCGCTimer to the Timer */
		
		if(LocTimerAB==TIMERA)
		{
			CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCTL_REG_OFFSET),BIT0);
		}
		else
		{
			CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCTL_REG_OFFSET),BIT8);
		}
		
		/*Disable the Timer */
		SET_BIT		(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCFG_REG_OFFSET),BIT2);
		CLEAR_BIT	(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCFG_REG_OFFSET),BIT1);
		CLEAR_BIT	(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCFG_REG_OFFSET),BIT0);
		/*Make the timer in individual mode*/
		
		if(LocTimerAB==TIMERA)
		{
			if(LocMode==GPT_ONE_SHOT_TIMER_MODE)
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT0);
				CLEAR_BIT		(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT1);
			}
			else if(LocMode==GPT_PRIODIC_TIMER_MODE)
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT1);
				CLEAR_BIT		(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT0);
			}
			else if(LocMode==GPT_CAPTURE_MODE)
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT1);
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT1);				
			}
			else
			{
			}
			*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAPR_REG_OFFSET)=0xFF;
			if(LocTimerType==TIMER16)
			{
				*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAV_REG_OFFSET)=LocTicks&0xFFFF;
			}
			else
			{
				*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAV_REG_OFFSET)=LocTicks;
			}
			
			if(GPT_CFG_COUNT_DIRECTION==GPT_TIMER_COUNT_DOWN)
			{
				CLEAR_BIT		(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT4);
			}
			else
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT4);
			}
			
			if(GPT_CFG_SNAPSHOT==GPT_SNAPSHOT_DISABLE)
			{
				CLEAR_BIT		(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT7);
			}
			else
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT7);				
			}
			
			if(GPT_CFG_INTERRUPT==GPT_INTERRUPT_ENABLE)
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT5);	
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT9);	
			}
			else
			{
				CLEAR_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT5);	
				CLEAR_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT9);					
			}
			

			
		}/*mode is set, Count direction is set , Snapshot is set , Interrupt is set, Ticks is set in case of TimerA*/
		else
		{
			
			if(LocMode==GPT_ONE_SHOT_TIMER_MODE)
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT0);
				CLEAR_BIT		(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT1);
			}
			else if(LocMode==GPT_PRIODIC_TIMER_MODE)
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT1);
				CLEAR_BIT		(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT0);
			}
			else if(LocMode==GPT_CAPTURE_MODE)
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT1);
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT1);				
			}
			else
			{
			}	
			
			*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBPR_REG_OFFSET)=0xFF;
			if(LocTimerType==TIMER16)
			{
				*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBV_REG_OFFSET)=LocTicks&0xFFFF;
			}
			else
			{
				*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBV_REG_OFFSET)=LocTicks;
			}
			if(GPT_CFG_COUNT_DIRECTION==GPT_TIMER_COUNT_DOWN)
			{
				CLEAR_BIT		(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT4);
			}
			else
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT4);
			}
		
			if(GPT_CFG_SNAPSHOT==GPT_SNAPSHOT_DISABLE)
			{
				CLEAR_BIT		(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT7);
			}
			else
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT7);				
			}			
			
			if(GPT_CFG_INTERRUPT==GPT_INTERRUPT_ENABLE)
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT5);	
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT9);	
			}
			else
			{
				CLEAR_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT5);	
				CLEAR_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT9);					
			}
		
		}/*mode is set, Count direction is set , Snapshot is set, Interrupt is set,Ticks is set in case of TimerB*/
		
		
		SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMICR_REG_OFFSET),BIT0);
		
		

	}
	
	}
}


/******************************************************************************
* \Syntax          : void Gpt_DisableNotification( Gpt_ChannelType Channel )        
* \Description     :                                                                                                
* \Sync\Async      :           
* \Reentrancy      :           
* \Parameters (in) :                      
* \Parameters (out):           
* \Return value:   : 
*******************************************************************************/
void Gpt_DisableNotification(Gpt_ChannelType ChannelId)
{
	if(ChannelId/TIMER_COUNT)
	{
		LocTimerAB=TIMERB;
		ChannelId-=TIMER_COUNT;
	}
	
	LocBaseAdd=GPT_TimerChannelBaseAddress[ChannelId];
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT0);
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT1);
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT2);
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT3);
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT4);
	
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT8);
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT9);
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT10);
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT11);
	
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT16);
}


/******************************************************************************
* \Syntax          : void Gpt_EnableNotification(Gpt_ChannelType ChannelId);        
* \Description     :                                                                                                
* \Sync\Async      :           
* \Reentrancy      :           
* \Parameters (in) :                      
* \Parameters (out):           
* \Return value:   : 
*******************************************************************************/
void Gpt_EnableNotification(Gpt_ChannelType ChannelId)
{
	if(ChannelId/TIMER_COUNT)
	{
		LocTimerAB=TIMERB;
		ChannelId-=TIMER_COUNT;
	}
	LocBaseAdd=GPT_TimerChannelBaseAddress[ChannelId];
	
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT0);
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT1);
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT2);
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT3);
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT4);
	
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT8);
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT9);
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT10);
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT11);
	
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT16);
}

/******************************************************************************
* \Syntax          : void Gpt_StartTimer(Gpt_ChannelType ChannelId,Gpt_ValueType Value);        
* \Description     :                                                                                                
* \Sync\Async      :           
* \Reentrancy      :           
* \Parameters (in) :                      
* \Parameters (out):           
* \Return value:   : 
*******************************************************************************/
void Gpt_StartTimer(Gpt_ChannelType ChannelId,Gpt_ValueType Value)
{
	if(ChannelId/TIMER_COUNT)
	{
		LocTimerAB=TIMERB;
		ChannelId-=TIMER_COUNT;
	}
	LocBaseAdd=GPT_TimerChannelBaseAddress[ChannelId];
	
	if(ChannelId/(TIMER_COUNT/2))
	{
		LocTimerType=TIMER32;
	}
	else
	{
		LocTimerType=TIMER16;
	}
	if(LocTimerAB==TIMERA)
	{
		
		SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT5);
		SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCTL_REG_OFFSET),BIT0);
		
			if(LocTimerType==TIMER16)
			{
				*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAV_REG_OFFSET)=Value&0xFFFF;
			}
			else
			{
				*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAV_REG_OFFSET)=Value;
			}
	}
	else
	{
		
		SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT5);
		SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCTL_REG_OFFSET),BIT8);
		
			if(LocTimerType==TIMER16)
			{
				*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBV_REG_OFFSET)=Value&0xFFFF;
			}
			else
			{
				*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBV_REG_OFFSET)=Value;
			}
	}	
	
	
	
}




/******************************************************************************
* \Syntax          : void Gpt_StopTimer(Gpt_ChannelType ChannelId);        
* \Description     :                                                                                                
* \Sync\Async      :           
* \Reentrancy      :           
* \Parameters (in) :                      
* \Parameters (out):           
* \Return value:   : 
*******************************************************************************/
void Gpt_StopTimer(Gpt_ChannelType ChannelId)
{
	if(ChannelId/TIMER_COUNT)
	{
		LocTimerAB=TIMERB;
		ChannelId-=TIMER_COUNT;
	}
	LocBaseAdd=GPT_TimerChannelBaseAddress[ChannelId];
	
if(LocTimerAB==TIMERA)
	{
		CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCTL_REG_OFFSET),BIT0);
	}
	else
	{
		CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCTL_REG_OFFSET),BIT8);
	}
}


void TIMER0A_Handler(void) __attribute__((used));
void TIMER0A_Handler(void)
{

		/*Call the funciton*/
		GptNotification[0]();
		
		/*Clear the flag*/
		SET_BIT(*(volatile uint32 *)(GPT_TimerChannelBaseAddress[0] + GPT_GPTMICR_REG_OFFSET),BIT0);
	

	
}

/**********************************************************************************************************************
 *  END OF FILE: FileName.c
 *********************************************************************************************************************/
