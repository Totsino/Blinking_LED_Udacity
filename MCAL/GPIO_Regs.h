 /******************************************************************************
 *
 * Module: Dio
 *
 * File Name: GPIO_Regs.h
 *
 * Author: Youssef Hussien
 ******************************************************************************/

#ifndef DIO_REGS_H
#define DIO_REGS_H

#include "Std_Types.h"

#define GPIO_PORTA_DATA_REG       (*((volatile uint32 *)0x400043FC))
#define GPIO_PORTB_DATA_REG       (*((volatile uint32 *)0x400053FC))
#define GPIO_PORTC_DATA_REG       (*((volatile uint32 *)0x400063FC))
#define GPIO_PORTD_DATA_REG       (*((volatile uint32 *)0x400073FC))
#define GPIO_PORTE_DATA_REG       (*((volatile uint32 *)0x400243FC))
#define GPIO_PORTF_DATA_REG       (*((volatile uint32 *)0x400253FC))

#endif /* DIO_REGS_H */
