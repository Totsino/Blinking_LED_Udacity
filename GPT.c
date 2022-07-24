/******************************************************************************
 *
 * Module: GPT
 *
 * File Name: GPT.c
 *
 * Description: Source file for TM4C123GH6PM Microcontroller - GPT Driver
 *
 * Author: seif adel
 ******************************************************************************/

#include "GPT.h"
#include "GPT__Regs.h"



Gpt_ConfigTypeDynamic Gpt_ConfigDynamic[GPT_CONFIGURED_CHANNLES];   
 
STATIC const Gpt_ConfigChannel * Gpt_GptChannels = NULL_PTR;
STATIC uint8 Gpt_Status = GPT_NOT_INITIALIZED;
STATIC Gpt_ModeType Gpt_Mode = GPT_MODE_NORMAL;
volatile uint32 * GPT_Ptr = NULL_PTR;     /* point to the required timer Registers base address */


/************************************************************************************
* Service Name: Gpt_Init
* Service ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in):ConfigPtr - Pointer to a selected configuration structure
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Initializes the hardware timer module.
************************************************************************************/
void Gpt_Init(const Gpt_ConfigType* ConfigPtr)
{

  {
    Gpt_Status = GPT_INITIALIZED;
    Gpt_GptChannels = ConfigPtr->Channels;
    Gpt_Mode = GPT_MODE_NORMAL;
    for(uint8 i=0;i<GPT_CONFIGURED_CHANNLES;i++)
    {
      Gpt_ConfigDynamic[i].Gpt_ChannelNum = (Gpt_GptChannels+i)->Gpt_ChannelNum; // store the ID for this channel
      Gpt_ConfigDynamic[i].Gpt_ChannelState = INITIALIZED; // init state of this channel INITIALIZED
      Gpt_ConfigDynamic[i].Gpt_EnableChannelWakeup = INTERRUPT_OFF; //disable interrupt wakeup for this channel 
      Gpt_ConfigDynamic[i].Gpt_EnableChannelNotification = INTERRUPT_OFF; // disable interrupt notification for this channel
      Gpt_ConfigDynamic[i].Gpt_DirtyBitWakeup = FALSE; // disable the dirty bit of wakeup (which determine whether the wakeup was enabled or not)
      Gpt_ConfigDynamic[i].Gpt_DirtyBitNotification = FALSE; // disable the dirty bit of notification(which determine whether the notification was enabled or not)
      
      ////////////////////////////////// next block to point on the base address of current configured timer ////////////////////////////////////
      uint8 temp = ((Gpt_GptChannels+i)->Gpt_ChannelNum) /2 ; // convert timers number from 0->24 to 0->12 to point on the base address 12 registers
      if(temp >= 0 && temp<= 7)
      {
        GPT_Ptr = (volatile uint32 *)GPT_TIMER0_BASE_ADDRESS +(1000 * temp);
      }
      else
      {
        GPT_Ptr = (volatile uint32 *)GPT_WIDE_TIMER2_BASE_ADDRESS +(1000 * (temp-8));
      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      if( ((Gpt_GptChannels+i)->Gpt_ChannelNum)%2 == 0) // we are in TIMER A
      {
          CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMCTL_REG_OFFSET),0)/* Clear the TAEN to disable the timer in the begining of configs */
          if((Gpt_GptChannels+i)->Gpt_ChannelRunningMode == GPT_ONE_SHOTT) // step 3 in initialization of one shot / periodic mode
          {
            *(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTAMR_REG_OFFSET) &= 0xFC;
            *(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTAMR_REG_OFFSET) |= 0x01; // select snap shot mode 
          }
          else 
          {
            *(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTAMR_REG_OFFSET) &= 0xFC;
            *(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTAMR_REG_OFFSET) |= 0x02; // select periodic mode
          }
          SET_BIT(*(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTAMR_REG_OFFSET),4);/* make the timer count up*/
      }      
      else // we are in timer B 
      {
          CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMCTL_REG_OFFSET),8);/* Clear the TBEN to disable the timer in the begining of configs */
          if((Gpt_GptChannels+i)->Gpt_ChannelRunningMode == GPT_ONE_SHOTT) // step 3 in initialization of one shot / periodic mode
          {
            *(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTBMR_REG_OFFSET) &= 0xFC;
            *(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTBMR_REG_OFFSET) |= 0x01; // select one shot mode 
          }
          else 
          {
            *(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTBMR_REG_OFFSET) &= 0xFC;
            *(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTBMR_REG_OFFSET) |= 0x02; // select periodic mode 
          }
          SET_BIT(*(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTBMR_REG_OFFSET),4);/* make the timer count up*/
      }
      *(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMCFG_REG_OFFSET) = 0x04; // operate the timer not concatenated mode 
    } // end of for loop here
    // only the step 5 and 7 in the initialization of one shot/ periodic mode is left for function starttime() to make it 
  }
} 
/************************************************************************************
* Service Name: Gpt_DeInit
* Service ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): None
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Deinitializes all hardware timer channels. 
************************************************************************************/       
#if (GptDeInitApi==STD_ON)       
void Gpt_DeInit(void)
{   

  {
    
    for(int i=0;i<GPT_CONFIGURED_CHANNLES;i++)
    {
      Gpt_ConfigDynamic[i].Gpt_EnableChannelWakeup = INTERRUPT_OFF; //disable interrupt wakeup for this channel 
      Gpt_ConfigDynamic[i].Gpt_EnableChannelNotification = INTERRUPT_OFF; // disable interrupt notification for this channel
      
      ////////////////////////////////// next block to point on the base address of current configured timer ////////////////////////////////////
      uint8 temp = ((Gpt_GptChannels+i)->Gpt_ChannelNum) /2 ; // convert timers number from 0->24 to 0->12 to point on the base address 12 registers
      if(temp >= 0 && temp<= 7)
      {
        GPT_Ptr = (volatile uint32 *)GPT_TIMER0_BASE_ADDRESS +(1000 * temp);
      }
      else
      {
        GPT_Ptr = (volatile uint32 *)GPT_WIDE_TIMER2_BASE_ADDRESS +(1000 * (temp-8));
      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      if( ((Gpt_GptChannels+i)->Gpt_ChannelNum)%2 == 0) // we are in TIMER A
      {
          CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMCTL_REG_OFFSET),0)/* Clear the TAEN to disable the timer in the begining of configs */
          *(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTAMR_REG_OFFSET) = 0x00000000; // default reset
      }      
      else // we are in timer B 
      {
          CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMCTL_REG_OFFSET),8)/* Clear the TBEN to disable the timer in the begining of configs */
          *(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTBMR_REG_OFFSET) = 0x00000000; // default reset 
      }
      *(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMCFG_REG_OFFSET) = 0x00000000; // default reset
    } // end of for loop here 
  }
} 
#endif
/************************************************************************************
* Service Name: Gpt_GetTimeElapsed
* Service ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): Channel - numeric identifier of the GPT channel
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: return the time already elapsed
************************************************************************************/
#if (GptTimeElapsedApi == STD_ON)
Gpt_ValueType Gpt_GetTimeElapsed(Gpt_ChannelType Channel)
{
  uint8 temp_index;
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
  bool found = FALSE;
  for(int i=0;i< GPT_CONFIGURED_CHANNLES;i++)
  {
    if(Channel == Gpt_ConfigDynamic[i]->Gpt_ChannelNum)
    {
      found = TRUE;
      temp_index = i;
      break;
    }
  }
  if(found==FALSE)
  {
    Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_GetTimeElapsed_SID,
                   GPT_E_PARAM_CHANNEL); 
    return 0;
  }
  #endif
  
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
    if(Gpt_Status == GPT_UNINITIALIZED)
    {
       Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_GetTimeElapsed_SID,
                       GPT_E_UNINIT); 
       return 0;
    }
  #endif
  switch(Gpt_ConfigDynamic[temp_index]->Gpt_ChannelState)
  {
    case INITIALIZED : return 0;
    break;
    case RUNNING : // return elapsed time
    break;
    case EXPIRED : // return target time 
    break;
    case STOPPED : // return elapsed time at moment of stop 
    break;
  }
}
#endif
/************************************************************************************
* Service Name: Gpt_GetTimeRemaining
* Service ID[hex]: 0x05
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): Channel - numeric identifier of the GPT channel
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Returns the time remaining until the target time is reached
************************************************************************************/
#if (GptTimeRemainingApi == STD_ON)
Gpt_ValueType Gpt_GetTimeRemaining(Gpt_ChannelType Channel)
{
  uint8 temp_index;
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
  bool found = FALSE;
  for(int i=0;i< GPT_CONFIGURED_CHANNLES;i++)
  {
    if(Channel == Gpt_ConfigDynamic[i]->Gpt_ChannelNum)
    {
      found = TRUE;
      temp_index = i;
      break;
    }
  }
  if(found==FALSE)
  {
    Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_GetTimeRemaining_SID,
                   GPT_E_PARAM_CHANNEL); 
    return 0;
  }
  #endif
  
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
    if(Gpt_Status == GPT_UNINITIALIZED)
    {
       Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_GetTimeRemaining_SID,
                       GPT_E_UNINIT); 
       return 0;
    }
  #endif
  switch(Gpt_ConfigDynamic[temp_index]->Gpt_ChannelState)
  {
    case INITIALIZED : return 0;
    break;
    case RUNNING : // return Remaining time
    break;
    case EXPIRED : return 0; 
    break;
    case STOPPED : // return Remaining time at moment of stop 
    break;
  }
}
#endif
/************************************************************************************
* Service Name: Gpt_StartTimer
* Service ID[hex]: 0x05
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): Channel - numeric identifier of the GPT channel 
* Parameters (in): Value - numeric value for the timer ticks 
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Start a timer channel. .
************************************************************************************/
void Gpt_StartTimer(Gpt_ChannelType Channel, Gpt_ValueType Value)
{
  uint8 temp_index;
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
  bool found = FALSE;
  for(int i=0;i< GPT_CONFIGURED_CHANNLES;i++)
  {
    if(Channel == Gpt_ConfigDynamic[i]->Gpt_ChannelNum)
    {
      found = TRUE;
      temp_index = i;
      break;
    }
  }
  if(found==FALSE)
  {
    Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_StartTimer_SID,
                   GPT_E_PARAM_CHANNEL); 
    return 0;
  }
  #endif
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
    if(Value ==0)
    {
      Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_StartTimer_SID,
                      GPT_E_PARAM_CHANNEL); 
      return 0;
    }
  #endif        
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
      if(Gpt_Status == GPT_UNINITIALIZED)
      {
        Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_StartTimer_SID,
                        GPT_E_UNINIT); 
      }
  #endif
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
      if(Gpt_ConfigDynamic[temp_index]->Gpt_ChannelState == RUNNING)
      {
        Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_StartTimer_SID,
                        GPT_E_BUSY); 
      }
  #endif
  Gpt_ConfigDynamic[temp_index]->Gpt_ChannelState = RUNNING; // change state of the channel into running
  
  ////////////////////////////////// next block to point on the base address of current configured timer ////////////////////////////////////
  uint8 temp = Channel /2 ; // convert timers number from 0->24 to 0->12 to point on the base address 12 registers
  if(temp >= 0 && temp<= 7)
  {
    GPT_Ptr = (volatile uint32 *)GPT_TIMER0_BASE_ADDRESS +(1000 * temp);
  }
  else
  {
    GPT_Ptr = (volatile uint32 *)GPT_WIDE_TIMER2_BASE_ADDRESS +(1000 * (temp-8));
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if( Channel %2 ==0) // Timer A
  {
    *(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTAILR_REG_OFFSET) = (uint16)Value; // put the value
     SET_BIT(*(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMCTL_REG_OFFSET),0)/* set the TAEN to enable the timer  */
     SET_BIT(*(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTAMR_REG_OFFSET),5)/* enable generating interrupt*/
     SET_BIT(*(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMIMR_REG_OFFSET),0)/* enable generating interrupt WHEN TIME OUT OCCUR */
  }
  else //Timer B
  {
    *(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTBILR_REG_OFFSET) = (uint16)Value; // put the value
     SET_BIT(*(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMCTL_REG_OFFSET),8)/* set the TAEN to enable the timer */
     SET_BIT(*(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMTBMR_REG_OFFSET),5)/* enable generating interrupt*/
     SET_BIT(*(volatile uint32 *)((volatile uint8 *)GPT_Ptr + GPT_GPTMIMR_REG_OFFSET),8)/* enable generating interrupt WHEN TIME OUT OCCUR */
  }
}
/************************************************************************************
* Service Name: Gpt_StopTimer
* Service ID[hex]: 0x06
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): Channel - Pumeric identifier of the GPT channel
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Stops a timer channel. .
************************************************************************************/
void Gpt_StopTimer(Gpt_ChannelType Channel)
{
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
  bool found = FALSE;
  for(int i=0;i< GPT_CONFIGURED_CHANNLES;i++)
  {
    if(Channel == Gpt_ConfigDynamic[i]->Gpt_ChannelNum)
    {
      found = TRUE;
      temp_index = i;
      break;
    }
  }
  if(found==FALSE)
    Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_StopTimer_SID,
                   GPT_E_PARAM_CHANNEL); 
  #endif
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
    if(Gpt_Status == GPT_UNINITIALIZED)
    {
      Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_StopTimer_SID,
                      GPT_E_UNINIT); 
    }
  #endif   
  if(Gpt_ConfigDynamic[temp_index]->Gpt_ChannelState == RUNNING)
  {
    return;
  }
  Gpt_ConfigDynamic[temp_index]->Gpt_ChannelState =STOPPED; // change the state into stopped
  
  // stop the timer here by accessing the hardware registers 
}
/************************************************************************************
* Service Name: Gpt_EnableNotification
* Service ID[hex]: 0x07
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): Channel - numeric identifier of the GPT channel
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Enables the interrupt notification for a channel (relevant in normal mode)
************************************************************************************/
#if (GPT_ENABLE_DISABLE_NOTIFICATION_API ==STD_ON)
void Gpt_EnableNotification(Gpt_ChannelType Channel)
{
  uint8 temp_index;
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
  bool found = FALSE;
  for(int i=0;i< GPT_CONFIGURED_CHANNLES;i++)
  {
    if(Channel == Gpt_ConfigDynamic[i]->Gpt_ChannelNum)
    {
      found = TRUE;
      temp_index = i;
      break;
    }
  }
  if(found==FALSE|| Gpt_GptChannels[temp_index]->Gpt_WakeupSource == WAKEUP_SOURCE_OFF)
    Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_EnableNotification_SID,
                   GPT_E_PARAM_CHANNEL); 
  #endif
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
    if(Gpt_Status == GPT_UNINITIALIZED)
    {
      Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_EnableNotification_SID,
                      GPT_E_UNINIT); 
    }
  #endif
    
   // review If no valid notification function is configured (GptNotification), the function Gpt_DisableNotification shall raise the error GPT_E_PARAM_CHANNEL    
    Gpt_ConfigDynamic[temp_index]->Gpt_EnableChannelNotification =INTERRUPT_ON;
    Gpt_ConfigDynamic[temp_index]->Gpt_DirtyBitNotification = TRUE;
}
#endif
/************************************************************************************
* Service Name: Gpt_DisableNotification
* Service ID[hex]: 0x08
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): Channel - numeric identifier of the GPT channel
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Disables the interrupt notification for a channel (relevant in normal mode)
************************************************************************************/
#if (GPT_ENABLE_DISABLE_NOTIFICATION_API ==STD_ON)
void Gpt_DisableNotification(Gpt_ChannelType Channel)
{
  uint8 temp_index;
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
  bool found = FALSE;
  for(int i=0;i< GPT_CONFIGURED_CHANNLES;i++)
  {
    if(Channel == Gpt_ConfigDynamic[i]->Gpt_ChannelNum)
    {
      found = TRUE;
      temp_index = i;
      break;
    }
  }
  if(found==FALSE|| Gpt_GptChannels[temp_index]->Gpt_WakeupSource == WAKEUP_SOURCE_ON)
    Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_DisableNotification_SID,
                   GPT_E_PARAM_CHANNEL); 
  #endif
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
    if(Gpt_Status == GPT_UNINITIALIZED)
    {
      Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_DisableNotification_SID,
                      GPT_E_UNINIT); 
    }
  #endif
    
  // review If no valid notification function is configured (GptNotification), the function Gpt_DisableNotification shall raise the error GPT_E_PARAM_CHANNEL  
  Gpt_ConfigDynamic[temp_index]->Gpt_EnableChannelNotification =INTERRUPT_OFF;
}
#endif
/************************************************************************************
* Service Name: Gpt_DisableWakeup
* Service ID[hex]: 0x0A
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): Channel - numeric identifier of the GPT channel
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Disables the wakeup interrupt of a channel (relevant in sleep mode).
************************************************************************************/
#if (GptWakeupFunctionalityApi==STD_ON)
void Gpt_DisableWakeup(Gpt_ChannelType Channel)
{
  uint8 temp_index;
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
  bool found = FALSE;
  for(int i=0;i< GPT_CONFIGURED_CHANNLES;i++)
  {
    if(Channel == Gpt_ConfigDynamic[i]->Gpt_ChannelNum)
    {
      found = TRUE;
      temp_index = i;
      break;
    }
  }
  if(found==FALSE || Gpt_GptChannels[temp_index]->Gpt_WakeupSource == WAKEUP_SOURCE_OFF) // the channel id is not from configured channels or its not wakeup source
    Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_DisableWakeup_SID,
                   GPT_E_PARAM_CHANNEL); 
  #endif
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
    if(Gpt_Status == GPT_UNINITIALIZED)
    {
      Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_DisableWakeup_SID,
                      GPT_E_UNINIT); 
    }
  #endif
    Gpt_ConfigDynamic[temp_index]->Gpt_EnableChannelWakeup = INTERRUPT_OFF;
}
#endif
/************************************************************************************
* Service Name: Gpt_EnableWakeup
* Service ID[hex]: 0x0B
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): Channel - numeric identifier of the GPT channel
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Enables the wakeup interrupt of a channel (relevant in sleep mode).
************************************************************************************/
#if (GptWakeupFunctionalityApi==STD_ON)
void Gpt_EnableWakeup(Gpt_ChannelType Channel)
{
  uint8 temp_index;
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
  bool found = FALSE;
  for(int i=0;i< GPT_CONFIGURED_CHANNLES;i++)
  {
    if(Channel == Gpt_ConfigDynamic[i]->Gpt_ChannelNum)
    {
      found = TRUE;
      temp_index = i;
      break;
    }
  }
  if(found==FALSE || Gpt_GptChannels[temp_index]->Gpt_WakeupSource == WAKEUP_SOURCE_OFF) // the channel id is not from configured channels or its not wakeup source
    Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_EnableWakeup_SID,
                   GPT_E_PARAM_CHANNEL); 
  #endif
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
    if(Gpt_Status == GPT_UNINITIALIZED)
    {
      Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_EnableWakeup_SID,
                      GPT_E_UNINIT); 
    }
  #endif
      Gpt_ConfigDynamic[temp_index]->Gpt_EnableChannelWakeup = INTERRUPT_ON;
      Gpt_ConfigDynamic[temp_index]->Gpt_DirtyBitWakeup = TRUE; 
}
#endif
/************************************************************************************
* Service Name: Gpt_SetMode
* Service ID[hex]: 0x0B
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): Mode - numeric identifier of the GPT driver mode
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: 
************************************************************************************/
#if (GptWakeupFunctionalityApi == STD_ON)
void Gpt_SetMode(Gpt_ModeType Mode)
{
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
  if(Gpt_Status == GPT_UNINITIALIZED)
  {
    Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_SetMode_SID,
                    GPT_E_UNINIT); 
  }
  #endif
  #if (GPT_DEV_ERROR_DETECT == STD_ON)
  if(Mode != GPT_MODE_NORMAL && Mode != GPT_MODE_SLEEP)
  {
    Det_ReportError(GPT_MODULE_ID, GPT_INSTANCE_ID, Gpt_SetMode_SID,
                    GPT_E_PARAM_MODE); 
  }
  #endif
  Gpt_Mode = Mode;
  if(Mode == GPT_MODE_NORMAL)
  {
    for(int i=0;i<GPT_CONFIGURED_CHANNLES;i++)
    {
      if(Gpt_ConfigDynamic[i]->Gpt_DirtyBitNotification == TRUE && Gpt_GptChannels[i]->Gpt_WakeupSource == WAKEUP_SOURCE_OFF)
          Gpt_EnableNotification(Gpt_GptChannels[i]->Gpt_ChannelNum);
      else 
          Gpt_DisableNotification(Gpt_GptChannels[i]->Gpt_ChannelNum);    
    }
  }
  else 
  {
    for(int i=0;i<GPT_CONFIGURED_CHANNLES;i++)
    {
      if(Gpt_ConfigDynamic[i]->Gpt_DirtyBitWakeup == TRUE && Gpt_GptChannels[i]->Gpt_WakeupSource == WAKEUP_SOURCE_ON)
          Gpt_EnableWakeup(Gpt_GptChannels[i]->Gpt_ChannelNum);
      else 
          Gpt_DisableWakeup(Gpt_GptChannels[i]->Gpt_ChannelNum); 
      if(Gpt_ConfigDynamic[i]->Gpt_ChannelState == RUNNING)
      {
         if(!(Gpt_GptChannels[i]->Gpt_WakeupSource == WAKEUP_SOURCE_ON && Gpt_ConfigDynamic[i]->Gpt_DirtyBitWakeup == TRUE ))
         {
           Gpt_StopTimer(Gpt_GptChannels[i]->Gpt_ChannelNum);
           Gpt_ConfigDynamic[i]->Gpt_ChannelState = STOPPED;
         }
      }
    }
  }
} 
#endif