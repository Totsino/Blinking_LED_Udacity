 /******************************************************************************
 *
 * Module: Port
 *
 * File Name: Port.h
 *
 * Author: Youssef Hussien
 ******************************************************************************/

#ifndef PORT_H
#define PORT_H



/*
 * Macros for PORT Status
 */
#define PORT_INITIALIZED                (1U)
#define PORT_NOT_INITIALIZED            (0U)

/* Standard types */
#include "Std_Types.h"


/* PORT Pre-Compile Configuration Header file */
#include "Port_Cfg.h"

   /* Include files */
#include "Common_Macros.h"
#include "Port_Regs.h"   

/******************************************************************************
 *                      API Service Id Macros                                 *
 ******************************************************************************/ 
   
/* Service ID for Port Init Channel */
#define PORT_Init_SID                   (uint8)0x00
   
 /* Service ID for port Set pin direction */
#define Port_SetPinDirection_SID        (uint8)0x01

/* Service ID for Port refresh port direction */
#define Port_RefreshPortDirection_SID   (uint8)0x02

/* Service ID for port get version info */
#define Port_GetVersionInfo_SID         (uint8)0x03

/* Service ID for port set pin mode */
#define Port_SetPinMode_SID             (uint8)0x04

   

/*******************************************************************************
 *                              Module Data Types                              *
 *******************************************************************************/

/*Type definition for Port_PinType used by Port APIs */
typedef uint8 Port_PinType;
      
/* Description: Enum to hold PIN direction */
typedef enum
{
    INPUT,OUTPUT
}Port_PinDirectionType;

/* Description: Enum to hold internal resistor type for PIN */
typedef enum
{
    OFF,PULL_UP,PULL_DOWN
}Port_InternalResistor;


/*Data type for the symbolic name of a port pin*/
typedef uint8 Port_PinType;

/*Data type for the symbolic name of a port Port*/
typedef uint8 Port_PortType;

/*Different port pin modes*/
typedef uint8 Port_PinModeType;

/*Direction changeability*/
typedef enum{
    DIRECTION_CHANGE_OFF,DIRECTION_CHANGE_ON
}Port_PinDirectionChangable;

/*mode changeability*/
typedef enum{
    MODE_CHANGE_OFF,MODE_CHANGE_ON
}Port_PinModeChangeability;

/* Description: Structure to configure each individual PIN */
typedef struct 
{
    Port_PortType port_num; 
    Port_PinType pin_num; 
    Port_PinDirectionType direction;
    Port_InternalResistor resistor;
    uint8 initial_value;
    Port_PinModeType pin_mode;
    Port_PinModeChangeability mode_changeable;
    Port_PinDirectionChangable direction_changeable;
}Port_ConfigChannel;

typedef struct Port_ConfigType
{
	Port_ConfigChannel Channels[PORT_CONFIGURED_CHANNLES];
} Port_ConfigType;
/*******************************************************************************
 *                      Function Prototypes                                    *
 *******************************************************************************/

/************************************************************************************
* Service Name: Port_Init
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): ConfigPtr - Pointer to configuration set
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Initializes the Port Driver module
************************************************************************************/
void Port_Init(const Port_ConfigType* ConfigPtr); 


/************************************************************************************
* Service Name: Port_SetPinDirection
* Sync/Async: Synchronous
* Reentrancy: reentrant
* Parameters (in): Pin          - Port Pin ID number
                   Direction    - Port Pin Direction 
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Sets the port pin direction
************************************************************************************/
void Port_SetPinDirection(Port_PinType Pin,Port_PinDirectionType Direction);


/************************************************************************************
* Service Name: Port_RefreshPortDirection
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): None     
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Refreshes port direction
************************************************************************************/
void Port_RefreshPortDirection(void);



/************************************************************************************
* Service Name: Port_SetPinMode
* Sync/Async: Synchronous
* Reentrancy: reentrant
* Parameters (in): Pin          - Port Pin ID number
                   Mode         - New Port Pin mode to be set on port pin. 
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Sets the port pin mode.
************************************************************************************/
void Port_SetPinMode(Port_PinType Pin,Port_PinModeType Mode);

extern const Port_ConfigType Port_Configuration;

#endif /* PORT_H */
