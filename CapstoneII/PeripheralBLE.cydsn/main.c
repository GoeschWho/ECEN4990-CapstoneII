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

void StackEventHandler( uint32 eventCode, void *eventParam );
void BLE_Connect();

/* define the test register to switch the PA/LNA hardware control pins */
#define CYREG_SRSS_TST_DDFT_CTRL 0x40030008

int main()
{
     CyGlobalIntEnable;   /* Enable global interrupts */
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    CyBle_Start( StackEventHandler );
    
    BLE_Connect();
    
    for(;;)
    {
        /* Place your application code here */
        CyBle_ProcessEvents();
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
            
        /* Add additional events as required */    
            
        /* default catch-all case */

        default:
        break;    
    }
}   

void BLE_Connect() {
   
    CYBLE_API_RESULT_T advertise_result;
    CYBLE_STATE_T ble_state;
    
    CyBle_ProcessEvents();
    
    advertise_result = CyBle_GappStartAdvertisement( CYBLE_ADVERTISING_FAST );
    CyBle_ProcessEvents();
    if (advertise_result == CYBLE_ERROR_OK ) {
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
            
/* [] END OF FILE */
