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
#include <stdio.h>

void StackEventHandler( uint32 eventCode, void *eventParam );
void BLE_Connect();

/* define the test register to switch the PA/LNA hardware control pins */
#define CYREG_SRSS_TST_DDFT_CTRL 0x40030008

/* Global Variables */
#define CYBLE_MAX_ADV_DEVICES 255

CYBLE_GAP_BD_ADDR_T peerAddr[CYBLE_MAX_ADV_DEVICES];
uint8 DevicesNearBy = 0;

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
    CYBLE_GAPC_ADV_REPORT_T advReport;
    uint8 i;
    
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
        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
            
            /*copy the advertising packet recieved to advReport*/
            advReport=*(CYBLE_GAPC_ADV_REPORT_T *)eventParam;
            
            if((DevicesNearBy <= CYBLE_MAX_ADV_DEVICES) && (advReport.eventType!=CYBLE_GAPC_SCAN_RSP))
            {
                for(i = 0u; i < CYBLE_GAP_BD_ADDR_SIZE; i++)
                {
                    peerAddr[DevicesNearBy].bdAddr[i] = advReport.peerBdAddr[i];
                }                    
                
                /* Increase the number of peer devices when 
                 * received aadvertising packet from different peer devices.*/
                ++DevicesNearBy;
                
            }
            
        break;
            
        /* default catch-all case */

        default:
        break;    
    }
}    

void BLE_Connect() {
    
    CYBLE_API_RESULT_T scan_result;
    CYBLE_API_RESULT_T connect_result;
    CYBLE_STATE_T ble_state;
    
    //uint8 peripheral_address[6] = "000001";
    //uint8 peripheral_add_type = 0;
   // CYBLE_GAP_BD_ADDR_T peripheral_addr = {{0x00,0xA0,0x50,0x00,0x00,0x01}, 0};
    CYBLE_GAP_BD_ADDR_T peripheral_addr = {{0x01,0x00,0x00,0x50,0xA0,0x00}, 0};
    
    CyBle_ProcessEvents();
    
    scan_result = CyBle_GapcStartScan( CYBLE_SCANNING_FAST );
    if (scan_result == CYBLE_ERROR_OK) {
          LED_GREEN_Write(0);  
    }
    CyBle_ProcessEvents();
    CyDelay(1000);
    CyBle_GapcStopScan();
    CyBle_ProcessEvents();
//    if (DevicesNearBy == 0) {
//        LED_BLUE_Write(0);   
//    }
    CyDelay(1000);
    connect_result = CyBle_GapcConnectDevice( &peripheral_addr );
    
    CyBle_ProcessEvents();
    if (connect_result == CYBLE_ERROR_OK) {
        LED_BLUE_Write(0); 
        CyDelay(1000);
        LED_BLUE_Write(1);
        CyDelay(1000);
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
