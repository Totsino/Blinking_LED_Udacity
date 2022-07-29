 /******************************************************************************
 *
 * Module: Dio
 *
 * File Name: Dio_Cfg.h
 *
 * Author: Youssef Hussien
 ******************************************************************************/

#ifndef DIO_CFG_H
#define DIO_CFG_H




/* Pre-compile option for presence of Dio_FlipChannel API */
#define DIO_FLIP_CHANNEL_API                (STD_ON)

/* Number of the configured Dio Channels */
#define DIO_CONFIGURED_CHANNLES              (1U)

/* Channel Index in the array of structures in Dio_PBcfg.c */
#define DioConf_LED1_CHANNEL_ID_INDEX        (uint8)0x00


/* DIO Configured Port ID's  */
#define DioConf_LED1_PORT_NUM                (Dio_PortType)5 /* PORTF */


/* DIO Configured Channel ID's */
#define DioConf_LED1_CHANNEL_NUM             (Dio_ChannelType)1 /* Pin 1 in PORTF */


#endif /* DIO_CFG_H */
