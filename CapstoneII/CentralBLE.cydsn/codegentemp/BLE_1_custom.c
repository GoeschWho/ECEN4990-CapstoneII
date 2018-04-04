/***************************************************************************//**
* \file CYBLE_custom.c
* \version 3.30
* 
* \brief
*  Contains the source code for the Custom Service.
* 
********************************************************************************
* \copyright
* Copyright 2014-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include "BLE_1_eventHandler.h"

#ifdef CYBLE_CUSTOM_SERVER

/* If any custom service with custom characterisctis is defined in the
* customizer's GUI their handles will be present in this array.
*/
/* This array contains attribute handles for the defined Custom Services and their characteristics and descriptors.
   The array index definitions are located in the CYBLE_custom.h file. */


#endif /* (CYBLE_CUSTOM_SERVER) */

#ifdef CYBLE_CUSTOM_CLIENT
    
static const CYBLE_UUID128_T cyBle_customUuid128[] = {
    /* Custom Service */
    { {0x08u, 0xB3u, 0x0Du, 0x39u, 0x7Fu, 0x98u, 0xE4u, 0xB9u, 0x76u, 0x47u, 0xBCu, 0x0Au, 0xB1u, 0x7Bu, 0xBEu, 0x15u} },
    /* Custom Characteristic */
    { {0xD5u, 0x14u, 0x9Fu, 0x80u, 0xC6u, 0xAEu, 0x2Au, 0x82u, 0x93u, 0x41u, 0xEAu, 0x48u, 0x43u, 0x30u, 0x18u, 0x70u} },
    /* Custom Descriptor */
    { {0x09u, 0xB1u, 0xC5u, 0x37u, 0xC6u, 0x9Eu, 0x13u, 0xBDu, 0x0Fu, 0x4Bu, 0xD2u, 0xA1u, 0xE4u, 0xB3u, 0x0Bu, 0x7Cu} },
};

static CYBLE_CUSTOMC_DESC_T cyBle_customCDesc[0x01u] = {

    /* Custom Descriptor */
    {
        CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Descriptor handle */ 
        (const CYBLE_UUID128_T *) &cyBle_customUuid128[2], /* 128-bit UUID */ 
        0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
    },
};

static CYBLE_CUSTOMC_CHAR_T cyBle_customCChar[0x01u] = {

    /* Custom Characteristic */
    {
        CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Characteristic handle */ 
        CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Characteristic end handle */ 
        (const CYBLE_UUID128_T *) &cyBle_customUuid128[1], /* 128-bit UUID */ 
        0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        0x00u, /* Characteristic properties */ 
        0x01u, /* Number of descriptors */ 
        &cyBle_customCDesc[0], /* Characteristic Descriptors */ 
    },
};

CYBLE_CUSTOMC_T cyBle_customCServ[CYBLE_CUSTOMC_SERVICE_COUNT] = {

    /* Custom Service */
    {
        CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Custom Service handle */ 
        (const CYBLE_UUID128_T *) &cyBle_customUuid128[0], /* 128-bit UUID */ 
        0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        0x01u, /* Number of characteristics */ 
        &cyBle_customCChar[0], /* Custom Service Characteristics */ 
    },
};


static uint16 cyBle_customDisServIndex;
static uint16 cyBle_customDisCharIndex;

#endif /* (CYBLE_CUSTOM_CLIENT) */


/****************************************************************************** 
* Function Name: CyBle_CustomInit
***************************************************************************//**
* 
*  This function initializes Custom Service.
* 
* \return
*  None
* 
******************************************************************************/
void CyBle_CustomInit(void)
{
    
#ifdef CYBLE_CUSTOM_CLIENT

    uint16 locServIndex;
    uint16 locCharIndex;
    uint16 locDescIndex;
    
    for(locServIndex = 0u; locServIndex < CYBLE_CUSTOMC_SERVICE_COUNT; locServIndex++)
    {
        if(cyBle_serverInfo[CYBLE_SRVI_CUSTOMS + locServIndex].range.startHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            for(locCharIndex = 0u; locCharIndex < cyBle_customCServ[locServIndex].charCount; locCharIndex++)
            {
                cyBle_customCServ[locServIndex].customServChar[locCharIndex].
                    customServCharHandle = 0u;
                
                for(locDescIndex = 0u; locDescIndex < 
                    cyBle_customCServ[locServIndex].customServChar[locCharIndex].descCount; 
                        locDescIndex++)
                {
                    cyBle_customCServ[locServIndex].customServChar[locCharIndex].
                        customServCharDesc[locDescIndex].descHandle = 0u;
                }
            }
        }
    }

#endif /* (CYBLE_CUSTOM_CLIENT) */
}


#ifdef CYBLE_CUSTOM_CLIENT

    
/******************************************************************************
* Function Name: CyBle_CustomcDiscoverServiceEventHandler
***************************************************************************//**
* 
*  This function is called on receiving a Read By Group Response event or 
*  Read response with 128-bit service uuid. 
* 
*  \param *discServInfo: The pointer to a service information structure.
* 
* \return
*  None
* 
******************************************************************************/
void CyBle_CustomcDiscoverServiceEventHandler(const CYBLE_DISC_SRVC128_INFO_T *discServInfo)
{
    uint16 j;
    uint8 flag = 0u;
    
    /* Services with 128 bit UUID have discServInfo->uuid equal to 0 and address to 
       128 uuid is stored in cyBle_customCServ.uuid128
    */
	for(j = 0u; (j < CYBLE_CUSTOMC_SERVICE_COUNT) && (flag == 0u); j++)
    {
        if(cyBle_customCServ[j].uuidFormat == CYBLE_GATT_128_BIT_UUID_FORMAT)
        {
            if(memcmp(cyBle_customCServ[j].uuid, &discServInfo->uuid.uuid128, CYBLE_GATT_128_BIT_UUID_SIZE) == 0u)
            {
                if(cyBle_serverInfo[j + (uint16)CYBLE_SRVI_CUSTOMS].range.startHandle == 
                   CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
                {
                    cyBle_serverInfo[j + (uint16)CYBLE_SRVI_CUSTOMS].range = discServInfo->range;
                    cyBle_disCount++;
                    flag = 1u;
                }
            }
        }
    }
}


/******************************************************************************
* Function Name: CyBle_CustomcDiscoverCharacteristicsEventHandler
***************************************************************************//**
* 
*  This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP
*  event. Based on the service index, an appropriate data structure is populated
*  using the data received as part of the callback.
* 
*  \param *discCharInfo: The pointer to a characteristic information structure.
*  \param discoveryService: The index of the service instance.
* 
* \return
*  None
* 
******************************************************************************/
void CyBle_CustomcDiscoverCharacteristicsEventHandler(uint16 discoveryService, const CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    uint16 locCharIndex;
    static CYBLE_GATT_DB_ATTR_HANDLE_T *customsLastEndHandle = NULL;
    static uint16 discoveryLastServ = 0u;    
    uint8 locReqHandle = 0u;

    /* Update last characteristic endHandle to declaration handle of this characteristic */
    if(customsLastEndHandle != NULL)
    {
        if(discoveryLastServ == discoveryService)
        {
            *customsLastEndHandle = discCharInfo->charDeclHandle - 1u;
        }
        customsLastEndHandle = NULL;
    }
    
    for(locCharIndex = 0u; (locCharIndex < cyBle_customCServ[discoveryService].charCount) && (locReqHandle == 0u); 
        locCharIndex++)
    {
        uint8 flag = 0u;
        
        /* Support 128 bit uuid */
        if((discCharInfo->uuidFormat == CYBLE_GATT_128_BIT_UUID_FORMAT) &&
           (cyBle_customCServ[discoveryService].customServChar[locCharIndex].uuidFormat == 
                CYBLE_GATT_128_BIT_UUID_FORMAT))
        {
            if(memcmp(cyBle_customCServ[discoveryService].customServChar[locCharIndex].uuid, 
                &discCharInfo->uuid.uuid128, CYBLE_GATT_128_BIT_UUID_SIZE) == 0u)
            {
                flag = 1u;
            }
        }
        
        /* And support 16 bit uuid */
        if((discCharInfo->uuidFormat == CYBLE_GATT_16_BIT_UUID_FORMAT) &&
           (cyBle_customCServ[discoveryService].customServChar[locCharIndex].uuidFormat == 
                CYBLE_GATT_16_BIT_UUID_FORMAT))
        {
            if(memcmp(cyBle_customCServ[discoveryService].customServChar[locCharIndex].uuid, 
                &discCharInfo->uuid.uuid16, CYBLE_GATT_16_BIT_UUID_SIZE) == 0u)
            {
                flag = 1u;
            }
        }
        
        if((flag == 1u) && 
           (cyBle_customCServ[discoveryService].customServChar[locCharIndex].customServCharHandle
                == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE))
        {
            cyBle_customCServ[discoveryService].customServChar[locCharIndex].customServCharHandle = 
                discCharInfo->valueHandle;
            cyBle_customCServ[discoveryService].customServChar[locCharIndex].properties = 
                discCharInfo->properties;
            /* Init pointer to characteristic endHandle */
            customsLastEndHandle = &cyBle_customCServ[discoveryService].customServChar[locCharIndex].
                                    customServCharEndHandle;
            /* Init service index of discovered characteristic */
            discoveryLastServ = discoveryService;
            locReqHandle = 1u;
        }
    }
    
    /* Init characteristic endHandle to Service endHandle.
       Characteristic endHandle will be updated to the declaration
       Handler of the following characteristic,
       in the following characteristic discovery procedure. */
    if(customsLastEndHandle != NULL)
    {
        *customsLastEndHandle = cyBle_serverInfo[cyBle_disCount].range.endHandle;
    }
}


/****************************************************************************** 
* Function Name: CyBle_GetCustomCharRange
***************************************************************************//**
* 
*  Returns a possible range of the current characteristic descriptor
*  which is pointed by custom service and char index.
* 
*  \param incrementIndex: Not zero value indicates that service and characteristic index
*                  should be incremented.
* \return
*  CYBLE_GATT_ATTR_HANDLE_RANGE_T range: the block of start and end handles.
* 
******************************************************************************/
CYBLE_GATT_ATTR_HANDLE_RANGE_T CyBle_CustomcGetCharRange(uint8 incrementIndex)
{
    CYBLE_GATT_ATTR_HANDLE_RANGE_T charRange = {CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE};

    do{
        if(incrementIndex != CYBLE_DISCOVERY_INIT)
        {
            if((cyBle_customDisCharIndex + 1u) < cyBle_customCServ[cyBle_customDisServIndex].charCount)
            {
                cyBle_customDisCharIndex++;
            }
            else 
            {
                if((cyBle_customDisServIndex + 1u) < CYBLE_CUSTOMC_SERVICE_COUNT)
                {
                    cyBle_customDisServIndex++;      /* Discover descriptors for next custom service */
                    /* Set characteristic index to first characteristic of custom service */
                    cyBle_customDisCharIndex = 0u;
                }
                else /* Increment general discovery index when custom characteristic discovery is done. */
                {
                    cyBle_disCount++;  
                    cyBle_customDisCharIndex++;
                }
            }
        }
        else    /* Increment indexes in the following loop */
        {
            cyBle_customDisServIndex = 0u;
            cyBle_customDisCharIndex = 0u;
            incrementIndex = 1u;
        }            
        /* Read characteristic range */
        if(cyBle_customDisCharIndex < (cyBle_customCServ[cyBle_customDisServIndex].charCount))
        {
            charRange.startHandle = cyBle_customCServ[cyBle_customDisServIndex].
                                customServChar[cyBle_customDisCharIndex].customServCharHandle + 1u;
            charRange.endHandle = cyBle_customCServ[cyBle_customDisServIndex].
                                customServChar[cyBle_customDisCharIndex].customServCharEndHandle;
        }
    }while(((charRange.startHandle <= cyBle_gattcDiscoveryRange.startHandle) || 
            (charRange.startHandle > cyBle_gattcDiscoveryRange.endHandle) ||
            (charRange.endHandle < cyBle_gattcDiscoveryRange.startHandle) ||
            (charRange.startHandle > charRange.endHandle)) && 
            (cyBle_customDisCharIndex < cyBle_customCServ[cyBle_customDisServIndex].charCount));
    
    return(charRange);
}


/******************************************************************************
* Function Name: CyBle_CustomcDiscoverCharDescriptorsEventHandler
***************************************************************************//**
* 
*  This function is called on receiving a CYBLE_EVT_GATTC_FIND_INFO_RSP event.
*  Based on the descriptor UUID, an appropriate data structure is populated using
*  the data received as part of the callback.
* 
*  \param *discDescrInfo: The pointer to a descriptor information structure.
* 
* \return
*  None
* 
******************************************************************************/
void CyBle_CustomcDiscoverCharDescriptorsEventHandler(const CYBLE_DISC_DESCR_INFO_T *discDescrInfo)
{
    uint8 locDescIndex;
    uint8 locReqHandle = 0u;

    for(locDescIndex = 0u; (locDescIndex < cyBle_customCServ[cyBle_customDisServIndex].
          customServChar[cyBle_customDisCharIndex].descCount) && (locReqHandle == 0u); locDescIndex++)
    {
        uint8 flag = 0u;
        
        if((discDescrInfo->uuidFormat == CYBLE_GATT_128_BIT_UUID_FORMAT) &&
           (cyBle_customCServ[cyBle_customDisServIndex].customServChar[cyBle_customDisCharIndex].
             customServCharDesc[locDescIndex].uuidFormat == CYBLE_GATT_128_BIT_UUID_FORMAT))
        {
            if(memcmp(cyBle_customCServ[cyBle_customDisServIndex].
                customServChar[cyBle_customDisCharIndex].customServCharDesc[locDescIndex].uuid, 
                &discDescrInfo->uuid.uuid128, CYBLE_GATT_128_BIT_UUID_SIZE) == 0u)
            {
                flag = 1u;
            }
        }
        
        if((discDescrInfo->uuidFormat == CYBLE_GATT_16_BIT_UUID_FORMAT) &&
           (cyBle_customCServ[cyBle_customDisServIndex].customServChar[cyBle_customDisCharIndex].
             customServCharDesc[locDescIndex].uuidFormat == CYBLE_GATT_16_BIT_UUID_FORMAT))
        {
            if(memcmp(cyBle_customCServ[cyBle_customDisServIndex].
                customServChar[cyBle_customDisCharIndex].customServCharDesc[locDescIndex].uuid, 
                &discDescrInfo->uuid.uuid16, CYBLE_GATT_16_BIT_UUID_SIZE) == 0u)
            {
                flag = 1u;
            }
        }
        
        if((flag == 1u) && 
           (cyBle_customCServ[cyBle_customDisServIndex].customServChar[cyBle_customDisCharIndex].
               customServCharDesc[locDescIndex].descHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE))
        {
            cyBle_customCServ[cyBle_customDisServIndex].customServChar[cyBle_customDisCharIndex].
                customServCharDesc[locDescIndex].descHandle = discDescrInfo->descrHandle;
            locReqHandle = 1u;
        }
    }
}

#endif /* (CYBLE_CUSTOM_CLIENT) */


/* [] END OF FILE */
