 /******************************************************************************
 *
 * Module: GPT
 *
 * File Name: GPT.c
 *
 * Author: Youssef hussien
 ******************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "GPT.h"


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
Gpt_ValueType 	LocTicks		;
Gpt_Mode				LocMode			;
uint32					LocBaseAdd	;
uint8   				LocTimerAB	;
uint8						LocTimerType;

/**********************************************************************************************************************
 *  GLOBAL DATA
 *********************************************************************************************************************/
static void (*GptNotification[GPT_MAX_CHANNELS]) (void);  /* Array of Ptrs to function */
static const Gpt_ConfigChannel * Channel;


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
	
	if(ConfigPtr != NULL_PTR)
	{
		Channel    = ConfigPtr->channels;
		for(LocCounter = 0 ;LocCounter<GPT_CFG_CONFIGURED_CHANNELS;LocCounter++)
		{
			
			/* ****************************************************************** */
			LocChannelId								=Channel[LocCounter].GptChannelId										;
			LocMode											=Channel[LocCounter].GptChannelMode									;
			LocTicks										=Channel[LocCounter].GptChannelTickValueMax					;
			LocBaseAdd									=GPT_TimerChannelBaseAddress[LocChannelId%12];
			GptNotification[LocChannelId%12]	=Channel[LocCounter].GptNotifications								;  
			/*Configuration parameters now are saved into the local variables and the base address of the timer is saved*/
			/* ******************************************************** */

		/* Total 24 ChannelID, first 12 for TimerA, second 12 For TimerB*/
		if(LocChannelId/12)
		{
			LocTimerAB=TIMERB;
			LocChannelId-=12;
		}
		else
		{
			LocTimerAB=TIMERA;
			
		}
		/* ******************************************************** */
	  /* 16 bit or 32 bit Timer(Wide or Normal) & Enable Timer clocks*/
		if(LocChannelId>5 && LocChannelId<12)
		{
			LocTimerType=TIMER32;
			uint8 TempChannelID = LocChannelId-6;
			SET_BIT(SYSCTL_RCGCW_REG,TempChannelID);
		}
		else
		{
			LocTimerType=TIMER16;
			SET_BIT(SYSCTL_RCGC_REG,LocChannelId);
		}
		
		/* ******************************************************** */
		/*Disable the Timer */
		if(LocTimerAB==TIMERA)
		{
			CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCTL_REG_OFFSET),BIT0);  /* TAEN */
		}
		else
		{
			CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCTL_REG_OFFSET),BIT8);   /* TBEN */
		}
		
		/* ******************************************************** */
		/*Make the timer in individual mode*/
		SET_BIT		(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCFG_REG_OFFSET),BIT2);
		CLEAR_BIT	(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCFG_REG_OFFSET),BIT1);
		CLEAR_BIT	(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCFG_REG_OFFSET),BIT0);      /* 0x4 */
	
		/* ******************************************************** */
		/*Configure Mode Via Mode Register*/		
		if(LocTimerAB==TIMERA)
		{
			if(LocMode==GPT_ONE_SHOT_TIMER_MODE)
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT0);
				CLEAR_BIT		(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT1);  /* 0x1 */
			}
			else if(LocMode==GPT_PRIODIC_TIMER_MODE)
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT1);
				CLEAR_BIT		(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT0);  /* 0x2 */
			}
			else if(LocMode==GPT_CAPTURE_MODE)
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT1);
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT1);	/* 0x3 */			
			}

			
		/* ******************************************************** */
			if(LocTimerType==TIMER16)
			{
				/* Prescaler 0xFF for 256 ticks, 209.7152ms Max Time */
				*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAPR_REG_OFFSET)=0xFF;
			}
			else
			{
				/* Prescaler 0xFFFF for 65536 ticks, 3.518 10^6s Max Time */
				*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAPR_REG_OFFSET)=0xFFFF;
			}
			
			/* ******************************************************** */
			/* Interval Load */
			*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAILR_REG_OFFSET)=LocTicks&0xFFFF;
			
			/* ******************************************************** */
			/* Counting Up or Down Via Count Direction Bit in Mode Register */
			if(GPT_CFG_COUNT_DIRECTION==GPT_TIMER_COUNT_DOWN)
			{
				CLEAR_BIT		(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT4);  /* Counting Up */
			}
			else
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT4);  /*Counting Down*/
			}
			
			/* ******************************************************** */
			/* Controlling Snapchot via Snapshot Mode Bit in Mode Register */
			if(GPT_CFG_SNAPSHOT==GPT_SNAPSHOT_DISABLE)
			{
				CLEAR_BIT		(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT7);
			}
			else
			{
				SET_BIT			(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT7);				
			}
			
			/* ******************************************************** */
			/* Controlling Interuppt Via Match Interrupt Enable and PWM Interrupt Enable Bits in Mode Register */
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
		  /* ******************************************************** */
			/* ******************************************************** */
			/* ******************************************************** */
			
			
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

			
			
			if(LocTimerType==TIMER16)
			{
				*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBPR_REG_OFFSET)=0xFF;
			}
			else
			{
				*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBPR_REG_OFFSET)=0xFFFF;

			}
			*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBILR_REG_OFFSET)=LocTicks&0xFFFF;
			
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
	if(ChannelId/12)
	{
		LocTimerAB=TIMERB;
		ChannelId-=12;
	}
	/* Clear all bits in Interrupt Mask Regsiter */
	LocBaseAdd=GPT_TimerChannelBaseAddress[ChannelId];
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT0);
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT1);
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT2);
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT3);
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT4);
		                                                                        /* 5-7 RESERVED */
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT8);
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT9);
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT10);
	CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT11);
		                                                                        /* 12-15 RESERVED */	
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
	if(ChannelId/12)
	{
		LocTimerAB=TIMERB;
		ChannelId-=12;
	}
	/* Set all Bits in Interrupt Mask Register */
	LocBaseAdd=GPT_TimerChannelBaseAddress[ChannelId];
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT0);
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT1);
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT2);
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT3);
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT4);
	                                                                        /* 5-7 RESERVED */
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT8);
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT9);
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT10);
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMIMR_REG_OFFSET),BIT11);
	                                                                        /* 12-15 RESERVED */	
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
	if(ChannelId/12)
	{
		LocTimerAB=TIMERB;
		ChannelId-=12;
	}
	LocBaseAdd=GPT_TimerChannelBaseAddress[ChannelId];
	
	if(LocChannelId>5 && LocChannelId<12)
	{
		LocTimerType=TIMER32;
	}
	else
	{
		LocTimerType=TIMER16;
	}
	if(LocTimerAB==TIMERA)
	{
		/* GPT Match Interrupt Enable bit in Mode Register */
		SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAMR_REG_OFFSET),BIT5);
		/* TAEN Enable */
		SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCTL_REG_OFFSET),BIT0);
		

			*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTAILR_REG_OFFSET)=Value&0xFFFF;

	}
	else
	{
		
		SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBMR_REG_OFFSET),BIT5);
		SET_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCTL_REG_OFFSET),BIT8);
		

		*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMTBILR_REG_OFFSET)=Value&0xFFFF;
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
	if(ChannelId/12)
	{
		LocTimerAB=TIMERB;
		ChannelId-=12;
	}
	LocBaseAdd=GPT_TimerChannelBaseAddress[ChannelId];

if(LocTimerAB==TIMERA)
	{
		CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCTL_REG_OFFSET),BIT0); /* TAEN */
	}
	else
	{
		CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)LocBaseAdd + GPT_GPTMCTL_REG_OFFSET),BIT8); /* TBEN */
	}
	
}


void TIMER0A_Handler(void) __attribute__((used));
void TIMER0A_Handler(void)
{

		/*Call the funciton*/
		GptNotification[0]();
		
		/*Clear the flag by writing 1 in Time Out Raw Interrupt bit in Interrupt Clear Register*/ 
		SET_BIT(*(volatile uint32 *)(GPT_TimerChannelBaseAddress[0] + GPT_GPTMICR_REG_OFFSET),BIT0);
	

	
}

/**********************************************************************************************************************
 *  END OF FILE: FileName.c
 *********************************************************************************************************************/
