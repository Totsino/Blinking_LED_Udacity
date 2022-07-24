 /******************************************************************************
 *
 * Module: NVIC
 *
 * File Name: NVIC.c
 *
 * Description: NVIC driver
 *
 * Author: Youssef hussien
 ******************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "Std_Types.h"
#include "NVIC.h"
#include "Mcu_Hw.h"
#include "Common_Macros.h"

/**********************************************************************************************************************
*  LOCAL MACROS CONSTANT\FUNCTION
*********************************************************************************************************************/
#define NVIC_GROUPING_SYSTEM_XXX    4
#define NVIC_GROUPING_SYSTEM_XXY    5
#define NVIC_GROUPING_SYSTEM_XYY    6
#define NVIC_GROUPING_SYSTEM_YYY    7

#define APINT_VECTKEY                0x05FA
#define PRIGROUP_FIELD_OFFSET        0x8u
#define APINT_VECTKEY_FIELD_OFFSET   16u
#define NVIC_REG_NUM_OF_PRI_FIELDS   4u
	

/**********************************************************************************************************************
 *  LOCAL DATA 
 *********************************************************************************************************************/
STATIC const Nvic_Config * Nvic_Interrupts = NULL_PTR;


/******************************************************************************
* \Syntax          : void Nvic_Init(void)                                      
* \Description     : initialize Nvic Module by parsing the Configuration 
*                    into Nvic registers                                    
*                                                                             
* \Sync\Async      : Synchronous                                               
* \Reentrancy      : Non Reentrant                                             
* \Parameters (in) : None                     
* \Parameters (out): None                                                      
* \Return value:   : None
*******************************************************************************/
void Nvic_Init(const Nvic_ConfigType * ConfigPtr)
{
	Nvic_Interrupts = ConfigPtr->interrupts;
	Nvic_IntType intNum;
	uint8 locGroup,locSubGroup,i,locGroupingField;
	uint32 enRegOffset,enBitOffset,priRegOffset,priBitOffset;
	/*TODO COnfigure Grouping\SubGrouping System in APINT register in SCB*/
	
	APINT = (APINT_VECTKEY<<APINT_VECTKEY_FIELD_OFFSET) |
	        (NVIC_GROUPING_SYSTEM<<PRIGROUP_FIELD_OFFSET);
	
	for(i=0;i< NVIC_ACTIVATED_INT_SIZE;i++)
	{
		intNum = Nvic_Interrupts[i].interruptNumber;
		locGroup = Nvic_Interrupts[i].groupPriority;
		locSubGroup = Nvic_Interrupts[i].subgroupPriority;
		
		/*TODO : Enable\Disable based on user configurations in ENx Nvic Registers */
	
        /*NVIC_ENx Register*/
    enRegOffset = (intNum/WORD_LENGTH_BITS)*WORD_LENGTH_BYTES;
		enBitOffset = intNum%WORD_LENGTH_BITS;
		GET_HWREG(NVIC_ENABLE_BASE_ADDRESS,enRegOffset) |= (1<<enBitOffset);
		
		/* Create Grouping Field */
#if (NVIC_GROUPING_SYSTEM == NVIC_GROUPING_SYSTEM_XXX)
	      locGroupingField = locGroup;
#elif (NVIC_GROUPING_SYSTEM == NVIC_GROUPING_SYSTEM_XXY) 
        locGroupingField = ((locGroup<<1)&0x6) | (locSubGroup&0x1);
#elif (NVIC_GROUPING_SYSTEM == NVIC_GROUPING_SYSTEM_XYY) 
        locGroupingField = ((locGroup<<2)&0x4) | (locSubGroup&0x3);
#elif (NVIC_GROUPING_SYSTEM == NVIC_GROUPING_SYSTEM_YYY) 
        locGroupingField = locSubGroup;
#else
     #error INVALID GROUPING SELECTION
#endif 	/*NVIC_GROUPING_SYSTEM*/
		
		/*TODO : Assign Group\Subgroup priority in PRIx Nvic Registers*/
		
	    priRegOffset = (intNum / NVIC_REG_NUM_OF_PRI_FIELDS)*WORD_LENGTH_BYTES;
	   	priBitOffset = 5+((intNum % NVIC_REG_NUM_OF_PRI_FIELDS) *8 );
	    GET_HWREG(NVIC_PRI_BASE_ADDRESS,priRegOffset) |= (locGroupingField << priBitOffset);
	
		
	}	
}

/**********************************************************************************************************************
 *  END OF FILE: Nvic.c
 *********************************************************************************************************************/