 /******************************************************************************
 *
 * Module: Port
 *
 * File Name: Port_Cfg.h
 *
 * Description: Configure file for TM4C123GH6PM Microcontroller - Port Driver.
 *
 * Author: Youssef Hussien
 ******************************************************************************/
 
#ifndef PORT_CFG_H
#define PORT_CFG_H


/* Pre-compile option for presence of port_FlipChannel API */
#define PORT_SET_PIN_DIRECTION                  (STD_ON)

/* Number of the configured Port Channels */
#define PORT_CONFIGURED_CHANNLES                (35U)

/* Number of the configured Port mods */
#define PORT_CONFIGURED_MODES                   (13U)

/* ports configuring with id */

#define PORTA (uint8)0
#define PORTB (uint8)1
#define PORTC (uint8)2
#define PORTD (uint8)3
#define PORTE (uint8)4
#define PORTF (uint8)5

/*pins configuring with id */

#define PIN0 (uint8)0
#define PIN1 (uint8)1
#define PIN2 (uint8)2
#define PIN3 (uint8)3
#define PIN4 (uint8)4
#define PIN5 (uint8)5
#define PIN6 (uint8)6
#define PIN7 (uint8)7

/* Modes configuration */

#define PortConfig_Mode_ADC 		(uint8)9
#define PortConfig_Mode_DIO 		(uint8)0
#define PortConfig_Mode_UART 		(uint8)1
#define PortConfig_Mode_SSI 		(uint8)2
#define PortConfig_Mode_I2C 		(uint8)3
#define PortConfig_Mode_M0PWM 		(uint8)4
#define PortConfig_Mode_M0FAULT 	(uint8)4
#define PortConfig_Mode_M1PWM 		(uint8)5
#define PortConfig_Mode_IDX 		(uint8)6
#define PortConfig_Mode_TIMER 		(uint8)7
#define PortConfig_Mode_CAN 		(uint8)8
#define PortConfig_Mode_USB 		(uint8)8
#define PortConfig_Mode_NMI 		(uint8)8



#endif 