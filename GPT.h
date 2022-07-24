/******************************************************************************
 *
 * Module: GPT
 *
 * File Name:  GPT.h
 *
 * Description: Header file for TM4C123GH6PM Microcontroller - GPT Driver
 *
 * Author: seif adel
 ******************************************************************************/

#ifndef GPT_H
#define GPT_H


/* GPT Instance Id */
#define GPT_INSTANCE_ID  (0U)


/*
 * Macros for GPT Status
 */
#define GPT_INITIALIZED                (1U)
#define GPT_NOT_INITIALIZED            (0U)

/* Standard AUTOSAR types */
#include "Std_Types.h"
#include "GPT__Regs.h"
#include "GPT_Cfg.h"

/* Non AUTOSAR files */
#include "Common_Macros.h"


#define GPT_MAX_CONFIGURED_CHANNLES     (24U)

extern const GPT_ConfigType  Gpt_Configuration;
extern const boolean VARIANT_POST_BUILD_SUPPORTED;
/******************************************************************************
 *                      API Service Id Macros                                 *
 ******************************************************************************/
/* Service ID for getting version of the GPT Driver. */
#define Gpt_GetVersionInfo_SID          (uint8)0x00 

/* Service ID for Initializes the GPT Driver. */
#define Gpt_Init_SID                    (uint8)0x01

/* Service ID for Initializes the GPT Driver.  */
#define Gpt_DeInit_SID                  (uint8)0x02

/* Service ID for getting the elapsed time. */
#define Gpt_GetTimeElapsed_SID          (uint8)0x03

/* Service ID for getting the remaining time. */
#define Gpt_GetTimeRemaining_SID        (uint8)0x04

/* Service ID for starting the timer. */
#define Gpt_StartTimer_SID              (uint8)0x05

/* Service ID for stopping the timer. */
#define Gpt_StopTimer_SID               (uint8)0x06

/* Service ID for enabling notification for the timer channel. */
#define Gpt_EnableNotification_SID      (uint8)0x07

/* Service ID for disabling notification for the timer channel. */
#define Gpt_DisableNotification_SID     (uint8)0x08

/* Service ID for setting the mode of the GPT driver. */
#define Gpt_SetMode_SID                 (uint8)0x09

/* Service ID for disabling wakeup for a channel. */
#define Gpt_DisableWakeup_SID           (uint8)0x0A

/* Service ID for enabling wakeup for a channel. */
#define Gpt_EnableWakeup_SID            (uint8)0x0B

/* Service ID for checking wakeup for a channel. */
#define Gpt_CheckWakeup_SID            (uint8)0x0C


/*******************************************************************************
 *                      DET Error Codes                                        *
 *******************************************************************************/
/* DET code to report that API service called without module initialization. */
#define GPT_E_UNINIT                  (uint8)0x0A

/* DET code to report that API service called when timer is running. */
#define GPT_E_BUSY                     (uint8)0x0B

/* DET code to report that init service called when already initialized. */
#define GPT_E_ALREADY_INITIALIZED      (uint8)0x0D

/* DET code to report that invalid channel ID. */
#define GPT_E_PARAM_CHANNEL            (uint8)0x14

/* DET code to report that invalid value. */
#define GPT_E_PARAM_VALUE              (uint8)0x15

/* DET code to report that invalid pointer. */
#define GPT_E_PARAM_POINTER            (uint8)0x16 
   
/* DET code to report that invalid mode. */
#define GPT_E_PARAM_MODE               (uint8)0x1F


/*******************************************************************************
 *                              Module Data Types                              *
 *******************************************************************************/
/* Type definition for GPT_ChannelType used by the GPT APIs */
typedef uint8 Gpt_ChannelType;

/* Type definition for Gpt_ValueType used by the GPT APIs */
typedef uint32 Gpt_ValueType;

/* Type definition for Gpt_ModeType used by the GPT APIs */
typedef enum
{
  GPT_MODE_NORMAL, Gpt_MODE_SLEEP
} Gpt_ModeType;

/* Type definition for Gpt_StateType used for each channel */
typedef enum
{
  INITIALIZED, RUNNING, STOPPED, EXPIRED
} Gpt_StateType;

/* Type definition for Gpt_InterruptOption used for each channel to enable interrupts */
typedef enum
{
  INTERRUPT_OFF, INTERRUPT_ON
} Gpt_InterruptOption;

typedef enum 
{
  WAKEUP_SOURCE_ON,WAKEUP_SOURCE_OFF
}Gpt_WakeupSourceOption;

/* Type definition for Gpt_InterruptOption used for each channel to enable interrupts */
typedef enum
{
  GPT_ONE_SHOTT, GPT_CONTINUOUS
} Gpt_RunningMode;

/* Description: Structure to configure each individual channel:
 *	1. The Id of the channel.
 *	2. The value of the channel (number of ticks).
 *      3. The channel status (initialized, running, stopped, expired).
 *      4. The channel wakeup interrupt option (on, off).
 *	5. The channel notification interrupt option (on, off).
 *	6. The channel running mode (one shott, continuous).
 */
typedef struct Gpt_ConfigChannel
{
  Gpt_ChannelType Gpt_ChannelNum;
  //Gpt_ValueType Gpt_ChannelValue; // this attribute send with function start_time()
  //Gpt_StateType Gpt_ChannelState; // all channels state in initialzation is (INITIALIZED)
  //Gpt_InterruptOption Gpt_EnableChannelWakeup; // all interrupt notifications must be disabled
  //Gpt_InterruptOption Gpt_EnableChannelNotification; // all wakeup notifications must be disabled
  Gpt_RunningMode Gpt_ChannelRunningMode; 
  Gpt_WakeupSourceOption Gpt_WakeupSource;
} Gpt_ConfigChannel;

/* Data Structure required for initializing the GPT Driver */
typedef struct Gpt_ConfigType
{
	Gpt_ConfigChannel Channels[GPT_CONFIGURED_CHANNLES];
} Gpt_ConfigType;

typedef struct Gpt_ConfigTypeDynamic
{
  Gpt_ChannelType Gpt_ChannelNum;
  Gpt_StateType Gpt_ChannelState;
  Gpt_InterruptOption Gpt_EnableChannelWakeup;
  Gpt_InterruptOption Gpt_EnableChannelNotification;
  boolean Gpt_DirtyBitWakeup;
  boolean Gpt_DirtyBitNotification;
}Gpt_ConfigTypeDynamic;

/*******************************************************************************
 *                      Function Prototypes                                    *
 *******************************************************************************/
/* 1) Function for GPT Initialization API */
void Gpt_Init(const Gpt_ConfigType* ConfigPtr);

/* 2) Function for GPT deInitialization API */
#if (GptDeInitApi==STD_ON)
void Gpt_DeInit(void);
#endif

/* 3) Function returns the time already elapsed. */
#if (GptTimeElapsedApi == STD_ON)
Gpt_ValueType Gpt_GetTimeElapsed(Gpt_ChannelType Channel);
#endif

/* 4) Function returns the time remaining. */
#if (GptTimeRemainingApi == STD_ON)
Gpt_ValueType Gpt_GetTimeRemaining(Gpt_ChannelType Channel);
#endif

/* 5) function starts a timer channel. */
void Gpt_StartTimer(Gpt_ChannelType Channel, Gpt_ValueType Value);

/* 6) function stops a timer channel. */
void Gpt_StopTimer(Gpt_ChannelType Channel); 

// 7) enables the interrupt notification for a channel (relevant in normal mode)
#if (GPT_ENABLE_DISABLE_NOTIFICATION_API ==STD_ON)
void Gpt_EnableNotification(Gpt_ChannelType Channel);
#endif

// 8) disables the interrupt notification for a channel (relevant in normal mode)
void Gpt_DisableNotification(Gpt_ChannelType Channel);

// 9) sets the operation mode of the GPT.
#if (GptWakeupFunctionalityApi == STD_ON)
void Gpt_SetMode(Gpt_ModeType Mode); 
#endif

// 10) Disables the wakeup interrupt of a channel (relevant in sleep mode).
#if (GptWakeupFunctionalityApi==STD_ON)
void Gpt_DisableWakeup(Gpt_ChannelType Channel);
#endif

// 11) enables the wakeup interrupt of a channel (relevant in sleep mode).
#if (GptWakeupFunctionalityApi==STD_ON)
void Gpt_EnableWakeup(Gpt_ChannelType Channel);
#endif

/* Returns the version information of this module*/ 
#if (GPT_VERSION_INFO_API == STD_ON)
void Gpt_GetVersionInfo(Std_VersionInfoType* VersionInfoPtr);
#endif


#define TIMER0_A  0
#define TIMER0_B  1
#define TIMER1_A  2
#define TIMER1_B  3
#define TIMER2_A  4
#define TIMER2_B  5
#define TIMER3_A  6
#define TIMER3_B  7
#define TIMER4_A  8
#define TIMER4_B  9
#define TIMER5_A  10
#define TIMER5_B  11
#define TIMER0_A_WIDE  12
#define TIMER0_B_WIDE  13
#define TIMER1_A_WIDE  14
#define TIMER1_B_WIDE  15
#define TIMER2_A_WIDE  16
#define TIMER2_B_WIDE  17
#define TIMER3_A_WIDE  18
#define TIMER3_B_WIDE  19
#define TIMER4_A_WIDE  20
#define TIMER4_B_WIDE  21
#define TIMER5_A_WIDE  22
#define TIMER5_B_WIDE  23


#endif /* GPT_H */