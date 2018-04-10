/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include <DS18B20.h>
#include <math.h>

void StackEventHandler( uint32 eventCode, void *eventParam );
void BLEConnect();
void BLEStayConnected();
void BLEUpdateTemp(uint8_t temperature);

/* define the test register to switch the PA/LNA hardware control pins */
#define CYREG_SRSS_TST_DDFT_CTRL 0x40030008

int main()
{
     CyGlobalIntEnable;   /* Enable global interrupts */

    /*  BLE SETUP   */
    CyBle_Start( StackEventHandler );
    //BLEConnect();
    
    for(;;)
    {
        /* Place your application code here */
        CyBle_ProcessEvents();
        
        BLEStayConnected();
    }
}


void StackEventHandler( uint32 eventCode, void *eventParam )
{
    
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
    uint8_t i = 0;
    
    //temp
    uint8_t scratchPad[9];
    int16_t raw;
    float fltemp;
    uint8_t temp;
    bool crc_passed = false;
    uint8_t crc;
    
    
    CyBle_ProcessEvents();
    
//    //testing BLE write
//    CyDelay(2000);
//    BLEUpdateTemp(i++);
    
    // testing temp
//    if (Temp_Reset(1)) LED_GREEN_Write(0);
    CyDelay(1000);
    while (!crc_passed) {
        Temp_RequestTemp(1);
        while (Temp_ReadBit(1) == 0) LED_GREEN_Write(0);
        LED_GREEN_Write(1);    
    
        Temp_ReadScratchPad(1,scratchPad);
        crc = Temp_CRC(scratchPad);
        crc_passed = (Temp_CRC(scratchPad) == scratchPad[8]) ? true : false;
    }
    LED_GREEN_Write(0);    
    
    raw = Temp_CalculateTemperature(scratchPad);
    fltemp = Temp_RawToFahrenheit(raw);
    
//    fltemp = Temp_GetTempF(1);
    temp = (uint8_t) fltemp;
    raw = (uint8_t) fltemp;
    BLEUpdateTemp(temp);
    
    while(1) {
        ble_state = CyBle_GetState();
        if (ble_state == CYBLE_STATE_CONNECTED ) {
            LED_BLUE_Write(0);  
            LED_GREEN_Write(1);
        }
        else {
            LED_BLUE_Write(1);
            apiResult = CyBle_GappStartAdvertisement( CYBLE_ADVERTISING_FAST );
            CyBle_ProcessEvents();
//            if (apiResult == CYBLE_ERROR_OK ) {
//                LED_GREEN_Write(0);   
//            }
        }
        CyBle_ProcessEvents();
    }
}

void BLEUpdateTemp(uint8_t temperature) {
    
    CYBLE_GATT_ERR_CODE_T gattResult;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T handleValuePair;
    CYBLE_GATT_VALUE_T value;
    CYBLE_GATT_DB_ATTR_HANDLE_T attrHandle;
    uint8_t *temp;
    
    // Temp Characteristic Settings
    temp = &temperature;
    gattResult = 0;
    value.val = temp;
    value.len = sizeof(*temp);
    //value.len = 1;
    attrHandle = 0x0012;
    handleValuePair.attrHandle = attrHandle;
    handleValuePair.value = value;
    
    gattResult = CyBle_GattsWriteAttributeValue(&handleValuePair,0u,NULL,CYBLE_GATT_DB_LOCALLY_INITIATED);
//    LED_GREEN_Write(1);
//    if (gattResult == 13u) {
//        LED_GREEN_Write(0);
//    }  
    
}
            
/* [] END OF FILE */
