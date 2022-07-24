 /******************************************************************************
 *
 * Module: Dio
 *
 * File Name: Dio.c
 *
 * Description: Source file for TM4C123GH6PM Microcontroller - Dio Driver
 *
 * Author: Youssef Hussien
 ******************************************************************************/

#include "GPIO_Regs.h"
#include "gpio.h"




STATIC const Dio_ConfigChannel * Dio_PortChannels = NULL_PTR;
STATIC uint8 Dio_Status = DIO_NOT_INITIALIZED;



/************************************************************************************
* Service Name: Dio_Init
* Service ID[hex]: 0x10
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): ConfigPtr - Pointer to post-build configuration data
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function to Initialize the Dio module.
************************************************************************************/
void Dio_Init(const Dio_ConfigType * ConfigPtr)
{
	Dio_Status = DIO_INITIALIZED;
	Dio_PortChannels = ConfigPtr->Channels;
}

/************************************************************************************
* Service Name: Dio_WriteChannel
* Service ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): ChannelId - ID of DIO channel.
*                  Level - Value to be written.
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function to set a level of a channel.
************************************************************************************/
void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level)
{
	volatile uint32 *Port_Ptr = NULL_PTR;
	
	if(DIO_CONFIGURED_CHANNLES <= ChannelId){
		/* ERROR */
	}
	
	
	switch(Dio_PortChannels[ChannelId].Port_Num){
		case 0: 
			Port_Ptr = &GPIO_PORTA_DATA_REG;
			break;
		case 1: 
			Port_Ptr = &GPIO_PORTB_DATA_REG;
			break;
		case 2: 
			Port_Ptr = &GPIO_PORTC_DATA_REG;
			break;
		case 3: 
			Port_Ptr = &GPIO_PORTD_DATA_REG;
			break;
		case 4: 
			Port_Ptr = &GPIO_PORTE_DATA_REG;
			break;
		case 5: 
			Port_Ptr = &GPIO_PORTF_DATA_REG;
			break;
	}
	
	if(Level == STD_HIGH){
		SET_BIT(*Port_Ptr,Dio_PortChannels[ChannelId].Ch_Num);
	}else if(Level == STD_LOW){
		CLEAR_BIT(*Port_Ptr,Dio_PortChannels[ChannelId].Ch_Num);
	}
}


/************************************************************************************
* Service Name: Dio_ReadChannel
* Service ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): ChannelId - ID of DIO channel.
* Parameters (inout): None
* Parameters (out): None
* Return value: Dio_LevelType
* Description: Function to return the value of the specified DIO channel.
************************************************************************************/
Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId)
{
	volatile uint32 *Port_Ptr = NULL_PTR;
	Dio_LevelType output = STD_LOW;
	
	if(DIO_CONFIGURED_CHANNLES <= ChannelId){
		/* ERROR */
	}
	
	
	switch(Dio_PortChannels[ChannelId].Port_Num){
		case 0: 
			Port_Ptr = &GPIO_PORTA_DATA_REG;
			break;
		case 1: 
			Port_Ptr = &GPIO_PORTB_DATA_REG;
			break;
		case 2: 
			Port_Ptr = &GPIO_PORTC_DATA_REG;
			break;
		case 3: 
			Port_Ptr = &GPIO_PORTD_DATA_REG;
			break;
		case 4: 
			Port_Ptr = &GPIO_PORTE_DATA_REG;
			break;
		case 5: 
			Port_Ptr = &GPIO_PORTF_DATA_REG;
			break;
	}
	if(BIT_IS_CLEAR(*Port_Ptr, Dio_PortChannels[ChannelId].Ch_Num)){
		output = STD_LOW;
	}else if(BIT_IS_SET(*Port_Ptr, Dio_PortChannels[ChannelId].Ch_Num)){
		output = STD_HIGH;
	}
	
	return output;
}



/************************************************************************************
* Service Name: Dio_FlipChannel
* Service ID[hex]: 0x11
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): ChannelId - ID of DIO channel.
* Parameters (inout): None
* Parameters (out): None
* Return value: Dio_LevelType
* Description: Function to flip the level of a channel and return the level of the channel after flip.
************************************************************************************/
#if (DIO_FLIP_CHANNEL_API == STD_ON)
Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelId)
{
	volatile uint32 *Port_Ptr = NULL_PTR;
	Dio_LevelType output = STD_LOW;
	
	if(DIO_CONFIGURED_CHANNLES <= ChannelId){
		/* ERROR */
	}
	
	
	switch(Dio_PortChannels[ChannelId].Port_Num){
		case 0: 
			Port_Ptr = &GPIO_PORTA_DATA_REG;
			break;
		case 1: 
			Port_Ptr = &GPIO_PORTB_DATA_REG;
			break;
		case 2: 
			Port_Ptr = &GPIO_PORTC_DATA_REG;
			break;
		case 3: 
			Port_Ptr = &GPIO_PORTD_DATA_REG;
			break;
		case 4: 
			Port_Ptr = &GPIO_PORTE_DATA_REG;
			break;
		case 5: 
			Port_Ptr = &GPIO_PORTF_DATA_REG;
			break;
	}
	
	if(BIT_IS_SET(*Port_Ptr,Dio_PortChannels[ChannelId].Ch_Num))
		{
			CLEAR_BIT(*Port_Ptr,Dio_PortChannels[ChannelId].Ch_Num);
			output = STD_LOW;
		}
		else
		{
			SET_BIT(*Port_Ptr,Dio_PortChannels[ChannelId].Ch_Num);
			output = STD_HIGH;
		}
		
		return output;
}
#endif
