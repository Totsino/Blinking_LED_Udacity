/**********************************************************************************************************************

 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  <GPT.h>
 *       Module:  GPT: General Purpose Timers
 *
 *  Description:  Header file for GPT     
 *  
 *********************************************************************************************************************/
#ifndef GPT_H
#define GPT_H

/**********************************************************************************************************************
 * INCLUDES
 *********************************************************************************************************************/
#include "tm4c123gh6pm_registers.h"
#include "Std_Types.h"
#include "Common_Macros.h"
#include "GPT_Cfg.h"
#include "GPT__Regs.h"

/**********************************************************************************************************************
 *  GLOBAL CONSTANT MACROS
 *********************************************************************************************************************/
#define GPT_MAX_CHANNELS 			24
#define GPT_TIMER_COUNT_DOWN 	0
#define GPT_TIMER_COUNT_UP 		1
#define	GPT_INDIVIDUAL				0
#define GPT_CONTACENATED			1
#define GPT_INTERRUPT_ENABLE	1
#define GPT_INTERRUPT_DISABLE	0
#define GPT_ENABLE						1
#define GPT_DISABLE						0
#define GPT_SNAPSHOT_ENABLE		1
#define GPT_SNAPSHOT_DISABLE	0
/**********************************************************************************************************************
 *  GLOBAL FUNCTION MACROS
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 *********************************************************************************************************************/
typedef uint32 	Gpt_ValueType;
typedef uint8		Gpt_Frequency;

typedef enum 
{
	GPT_TIMER_A_0_16_32_BIT					,
	GPT_TIMER_A_1_16_32_BIT					,
	GPT_TIMER_A_2_16_32_BIT					,
	GPT_TIMER_A_3_16_32_BIT					,
	GPT_TIMER_A_4_16_32_BIT					,
	GPT_TIMER_A_5_16_32_BIT					,
	GPT_WIDE_TIMER_A_0_32_64_BIT		,
	GPT_WIDE_TIMER_A_1_32_64_BIT		,
	GPT_WIDE_TIMER_A_2_32_64_BIT		,
	GPT_WIDE_TIMER_A_3_32_64_BIT		,
	GPT_WIDE_TIMER_A_4_32_64_BIT		,
	GPT_WIDE_TIMER_A_5_32_64_BIT		,
	
	GPT_TIMER_B_0_16_32_BIT					,
	GPT_TIMER_B_1_16_32_BIT					,
	GPT_TIMER_B_2_16_32_BIT					,
	GPT_TIMER_B_3_16_32_BIT					,
	GPT_TIMER_B_4_16_32_BIT					,
	GPT_TIMER_B_5_16_32_BIT					,
	GPT_WIDE_TIMER_B_0_32_64_BIT		,
	GPT_WIDE_TIMER_B_1_32_64_BIT		,
	GPT_WIDE_TIMER_B_2_32_64_BIT		,
	GPT_WIDE_TIMER_B_3_32_64_BIT		,
	GPT_WIDE_TIMER_B_4_32_64_BIT		,
	GPT_WIDE_TIMER_B_5_32_64_BIT		,
}Gpt_ChannelType;



typedef enum
{
	GPT_MODE_NORMAL	,
	GPT_MODE_SLEEP	,
}Gpt_ModeType;



typedef enum
{
	GPT_ONE_SHOT_TIMER_MODE=0	,
	GPT_PRIODIC_TIMER_MODE		,
	GPT_CAPTURE_MODE					,
	GPT_RTC_TIMER_MODE				,
	GPT_PWM_TIMER_MODR				,
}Gpt_Mode;

typedef enum
{
	GPT_EDGE_TIME_TIMER_MODE	=0x03,
	GPT_EDGE_COUNT_TIMER_MODE	=0x13,
}Gpt_CaptureMode;



typedef struct
{
	Gpt_ChannelType GptChannelId						;
	Gpt_Frequency		GptChannelTickFrequency	;
	Gpt_ValueType		GptChannelTickValueMax	;
	Gpt_Mode				GptChannelMode					;
	void						(*GptNotifications)(void);
	
}Gpt_ConfigChannel;

typedef struct
{
	Gpt_ConfigChannel channels[GPT_CFG_CONFIGURED_CHANNELS];
}Gpt_ConfigType;

extern const Gpt_ConfigType GptConfigArr;
/**********************************************************************************************************************
 *  GLOBAL DATA PROTOTYPES
 *********************************************************************************************************************/

 
/**********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/
/******************************************************************************
Syntax          :   void Gpt_Init(const Gpt_ConfigType* ConfigPtr) 
Description     :    
Sync\Async      : 
Reentrancy      : 
Parameters (in) : 
Parameters (out):   
Return value    : 
*******************************************************************************/
void Gpt_Init(const Gpt_ConfigType* ConfigPtr);


/******************************************************************************
* \Syntax          : void Gpt_DisableNotification( Gpt_ChannelType Channel )        
* \Description     :                                                                                                
* \Sync\Async      :           
* \Reentrancy      :           
* \Parameters (in) :                      
* \Parameters (out):           
* \Return value:   : 
*******************************************************************************/
void Gpt_DisableNotification(Gpt_ChannelType ChannelId);

/******************************************************************************
* \Syntax          : void Gpt_EnableNotification(Gpt_ChannelType ChannelId);        
* \Description     :                                                                                                
* \Sync\Async      :           
* \Reentrancy      :           
* \Parameters (in) :                      
* \Parameters (out):           
* \Return value:   : 
*******************************************************************************/
void Gpt_EnableNotification(Gpt_ChannelType ChannelId);

/******************************************************************************
* \Syntax          : void Gpt_StartTimer(Gpt_ChannelType ChannelId,Gpt_ValueType Value);        
* \Description     :                                                                                                
* \Sync\Async      :           
* \Reentrancy      :           
* \Parameters (in) :                      
* \Parameters (out):           
* \Return value:   : 
*******************************************************************************/
void Gpt_StartTimer(Gpt_ChannelType ChannelId,Gpt_ValueType Value);

/******************************************************************************
* \Syntax          : void Gpt_StopTimer(Gpt_ChannelType ChannelId);        
* \Description     :                                                                                                
* \Sync\Async      :           
* \Reentrancy      :           
* \Parameters (in) :                      
* \Parameters (out):           
* \Return value:   : 
*******************************************************************************/
void Gpt_StopTimer(Gpt_ChannelType ChannelId);










#endif  /* FILE_NAME_H */

/**********************************************************************************************************************
 *  END OF FILE: Std_Types.h
 *********************************************************************************************************************/
