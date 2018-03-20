/*******************************************************************************
* File Name: FAN3.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_FAN3_ALIASES_H) /* Pins FAN3_ALIASES_H */
#define CY_PINS_FAN3_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define FAN3_0			(FAN3__0__PC)
#define FAN3_0_PS		(FAN3__0__PS)
#define FAN3_0_PC		(FAN3__0__PC)
#define FAN3_0_DR		(FAN3__0__DR)
#define FAN3_0_SHIFT	(FAN3__0__SHIFT)
#define FAN3_0_INTR	((uint16)((uint16)0x0003u << (FAN3__0__SHIFT*2u)))

#define FAN3_INTR_ALL	 ((uint16)(FAN3_0_INTR))


#endif /* End Pins FAN3_ALIASES_H */


/* [] END OF FILE */
