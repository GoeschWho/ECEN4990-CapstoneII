/* ========================================
 *
 * Copyright Megan Bird, 2018
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Megan Bird.
 *
 * ========================================
*/

#include <project.h>
#include <DS18B20.h>
#include <math.h>

void StackEventHandler( uint32 eventCode, void *eventParam );
void BLEConnect();
void BLEStayConnected();
void BLEUpdateDBTemp(float32 temperature);
void UpdateTemp(uint8_t n);

/* define the test register to switch the PA/LNA hardware control pins */
#define CYREG_SRSS_TST_DDFT_CTRL 0x40030008

int main()
{
    uint16_t i = 0;
     CyGlobalIntEnable;   /* Enable global interrupts */

    /*  BLE SETUP   */
    CyBle_Start( StackEventHandler );
    
    for(;;)
    {
        CyBle_ProcessEvents();
        
        // Advertise if disconnected
        BLEStayConnected();
        
        // Update temperature value
        if (i++ == 0) {
            UpdateTemp(1);
        }
        else if (i > 20000) {
            i = 0;
        }
    }
}

void StackEventHandler( uint32 eventCode, void *eventParam ) {
    
    switch( eventCode )
    {
        /* Generic events */

        case CYBLE_EVT_STACK_ON:
            /* Configure the Link Layer to automatically switch PA
             * control pin P3[2] and LNA control pin P3[3] */
            CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLESS_RF_CONFIG), 0x0331);
            CY_SET_XTND_REG32((void CYFAR *)(CYREG_SRSS_TST_DDFT_CTRL), 0x80000302);
            /* CyBle_GappStartAdvertisement( CYBLE_ADVERTISING_FAST ); */
        break;
            
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:          

        break;   
            
        /* default catch-all case */

        default:
        break;    
    }
}   

void BLEConnect() {
   
    CYBLE_API_RESULT_T apiResult;
    CYBLE_STATE_T ble_state;

    // Initiate GAP Connection
    CyBle_ProcessEvents();
    
    apiResult = CyBle_GappStartAdvertisement( CYBLE_ADVERTISING_FAST );
    CyBle_ProcessEvents();
    if (apiResult == CYBLE_ERROR_OK ) {
        LED_GREEN_Write(0);   
    }
    
    while(1) {
        ble_state = CyBle_GetState();
        if (ble_state == CYBLE_STATE_CONNECTED ) {
            LED_BLUE_Write(0);   
        }
        else {
            LED_BLUE_Write(1);
        }
        CyBle_ProcessEvents();
    }
    
}

void BLEStayConnected() {
   
    CYBLE_API_RESULT_T apiResult;
    CYBLE_STATE_T ble_state;
    
    CyBle_ProcessEvents();
    
    ble_state = CyBle_GetState();
    if (ble_state == CYBLE_STATE_CONNECTED ) {
        LED_BLUE_Write(0);  
        //LED_GREEN_Write(1);
    }
    else {
        LED_BLUE_Write(1);
        apiResult = CyBle_GappStartAdvertisement( CYBLE_ADVERTISING_FAST );
        CyBle_ProcessEvents();
        if (apiResult == CYBLE_ERROR_OK ) {
            //LED_GREEN_Write(0);   
        }
    }
    CyBle_ProcessEvents();
}

void BLEUpdateDBTemp(float32 temperature) {
    
    CYBLE_GATT_HANDLE_VALUE_PAIR_T handleValuePair;
    CYBLE_GATT_VALUE_T value;
    CYBLE_GATT_DB_ATTR_HANDLE_T attrHandle;
    uint8_t bytes[sizeof(float32)];
    
    // Temp Characteristic Settings
    *(float*)(bytes) = temperature;
    value.len = 1;
    attrHandle = 0x0012;
    handleValuePair.attrHandle = attrHandle;
    handleValuePair.value = value;
    
    for (uint8_t i = 0; i < 4; i++) {
        handleValuePair.value.val = &bytes[i];
        CyBle_GattsWriteAttributeValue(&handleValuePair,i,NULL,CYBLE_GATT_DB_LOCALLY_INITIATED);
        CyBle_GattsNotification(cyBle_connHandle,&handleValuePair);
    }
}

void UpdateTemp(uint8_t n) {
    
    float32 fltemp;
    
    
    Temp_RequestTemp(n);
    while (Temp_ReadBit(1) == 0) LED_GREEN_Write(0);
    LED_GREEN_Write(1);    
    fltemp = Temp_GetTempF(1);
    BLEUpdateDBTemp(fltemp);
}
            
/* [] END OF FILE */
