	 /******************************************************************************
 *
 * Module: GPT
 *
 * File Name: GPT_Cfg.h
 *
 * Description: Pre-Compile Configuration Header file for TM4C123GH6PM Microcontroller - GPT Driver
 *
 * Author: seif adel
 ******************************************************************************/

#ifndef GPT_CFG_H
#define GPT_CFG_H

/*
 * Module Version 1.0.0
 */
#define GPT_CFG_SW_MAJOR_VERSION              (1U)
#define GPT_CFG_SW_MINOR_VERSION              (0U)
#define GPT_CFG_SW_PATCH_VERSION              (0U)

/*
 * AUTOSAR Version 4.0.3
 */
#define GPT_CFG_AR_RELEASE_MAJOR_VERSION     (4U)
#define GPT_CFG_AR_RELEASE_MINOR_VERSION     (0U)
#define GPT_CFG_AR_RELEASE_PATCH_VERSION     (3U)

/* Pre-compile option for Development Error Detect */
#define GPT_DEV_ERROR_DETECT                (STD_ON)

/* function Gpt_SetMode is only available if the configurationparameter GptReportWakeupSource is enabled */
#define GPT_WAKEUP_FUNCTIONALITY_API               (STD_ON)

/* Pre-compile option for enable or disable notification */
#define GPT_ENABLE_DISABLE_NOTIFICATION_API     (STD_ON)

/* Pre-compile option for Version Info API */
#define GPT_VERSION_INFO_API                (STD_ON)
 
/* Pre-compile option for DeInit API */
#define GptDeInitApi                (STD_ON)

/* Pre-compile option for GetTimeElapsed API */
#define GptTimeElapsedApi         (STD_ON)
   
/* Pre-compile option for GptTimeRemainingApi API */
#define GptTimeRemainingApi         (STD_ON)
 
/* Pre-compile option to support GPT as wakeup source */
#define GptReportWakeupSource       (STD_ON)

/* Number of the configured GPT Channels */
#define GPT_CONFIGURED_CHANNLES                  (3U)
   

#endif /* GPT_CFG_H */