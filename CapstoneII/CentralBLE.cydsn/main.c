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
#include <RA8875.h>
#include <math.h>

/*  Declarations    */
void StackEventHandler( uint32 eventCode, void *eventParam );
void LCDInit(void);
void LCDInitDemo();
void LCDTouchscreenDemo();
void GUIInit();
void PrintInt(uint8_t n);
void PrintFloat(float n);
void RegReadTest();
void BLEConnect();
void FanTest();

 CYBLE_CONN_HANDLE_T                     connHandle;

/* define the test register to switch the PA/LNA hardware control pins */
#define CYREG_SRSS_TST_DDFT_CTRL 0x40030008

/* Global Variables */
#define CYBLE_MAX_ADV_DEVICES 255

CYBLE_GAP_BD_ADDR_T peerAddr[CYBLE_MAX_ADV_DEVICES];
uint8 DevicesNearBy = 0;

int main()
{
    CyGlobalIntEnable;   /* Enable global interrupts */
    
    /*  LCD Setup   */
    LCDInit();
    //RegReadTest();
    //CyDelay(10000);
    //LCDInitDemo();
    //LCDTouchscreenDemo();
    //RegReadTest();
    GUIInit();
    
    /* BLE Setup    */
    CyBle_Start( StackEventHandler );
    BLEConnect();
    
    /* Fan Test     */
    //FanTest();
    
    for(;;)
    {
        CyBle_ProcessEvents();
        //CyBle_GattcStartDiscovery();
    }
}


void StackEventHandler( uint32 eventCode, void *eventParam ) {
    
     //------ Declarations ------/
    // LCD Strings
    char ok[] = "CYBLE_ERROR_OK";
    char error[] = "CYBLE_ERROR_OCCURED";
    char param[] = "INVALID PARAMTER";
    char operation[] = "INVALID OPERATION";
    char serviceCount[] = "Service Count: ";
    char dataLength[] = "Data Length: ";
    char space[] = " ";
    char notification[] = "Notification Received";
    
    CYBLE_GAPC_ADV_REPORT_T                 advReport;
    CYBLE_GATTC_READ_BY_TYPE_REQ_T          readByTypeReqParam;
    CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T    readResponse;
    CYBLE_GATT_ATTR_HANDLE_RANGE_T          range;
    CYBLE_UUID_T                            uuid;
    CYBLE_API_RESULT_T                      apiResult;
   
    uint8                                   i;
    uint8_t fl_bytes[4];
    
    
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
            
        /* This event is received when connection is established */
        case CYBLE_EVT_GATT_CONNECT_IND:
  
             /* Retrieve BLE connection handle */
             connHandle = *(CYBLE_CONN_HANDLE_T *) eventParam;
            
         break;
            
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
                /*Start to dicovery the servioes of the serve after connection*/
                CyBle_GattcStartDiscovery(cyBle_connHandle);  
        break;
            
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
           // printf("\r\nDiscovery complete.\r\n");
            
            range.startHandle   = 0x0001;
            range.endHandle     = 0xFFFF;
            uuid.uuid16         = 0x14BF; 
   
            readByTypeReqParam.range = range;
            readByTypeReqParam.uuid = uuid;
            readByTypeReqParam.uuidFormat = 0x01;
            
            //textWrite(serviceCount,strlen(serviceCount));
            //PrintInt(CYBLE_SRVI_COUNT);
            
            //apiResult = CyBle_GattcDiscoverAllCharacteristics(connHandle, range);
   
            apiResult = CyBle_GattcReadUsingCharacteristicUuid(cyBle_connHandle,&readByTypeReqParam);
            
//            if ( apiResult == CYBLE_ERROR_OK )
//                textWrite(ok,strlen(ok));
//            else if ( apiResult == CYBLE_ERROR_INVALID_PARAMETER )
//                textWrite(param,strlen(param));
//            else if ( apiResult == CYBLE_ERROR_INVALID_OPERATION )
//                textWrite(operation,strlen(operation));
        break;
        
        case CYBLE_EVT_GATTC_READ_BY_TYPE_RSP:
            
            readResponse = *(CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T *) eventParam;
            
            textWrite(dataLength,strlen(dataLength));
            PrintInt(readResponse.attrData.length - 2);
            
            textSetCursor(100,200);
            textEnlarge(10);
               
            for( i=2 ; i < readResponse.attrData.length; i++)
            {
                PrintInt(readResponse.attrData.attrValue[i]);
                textWrite(space,strlen(space));
                
                fl_bytes[i-2] = readResponse.attrData.attrValue[i];
                
                //PrintInt(readResponse.attrData.attrValue[2]);
                //PrintFloat(readResponse.attrData.attrValue[2]);
            }
            
            // convert 4 bytes to float
            PrintFloat( *(float32*)(fl_bytes) );
            
        break;
                
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
                
            textWrite(notification,strlen(notification));
                
            apiResult = CyBle_GattcReadUsingCharacteristicUuid(cyBle_connHandle,&readByTypeReqParam);
                
        break;
            
        /* default catch-all case */

        default:
        break;    
    }
}    

void LCDInit() {
    
    LCDSPI_Start();
    
    //------ LCD Setup ------//  
    
    rst0_m_Write(1u);
    
    begin(RA8875_800x480);
    displayOn(true);
    GPIOX(true);      // Enable TFT - display enable tied to GPIOX
    
    PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight  
    PWM1out(255);
    
    LCD_int_Write(1u); 
}

void LCDInitDemo() {
    
    //------ Declarations ------/
    // LCD Strings
    char welcome1[] = "Megan Bird";
    char welcome2[] = "ECEN 4350";
    char welcome3[] = "Fall 2017";
    char ble[] = "BLE_Central";
    
    uint16 myDelay = 200;
    
    //----- Color Demo -----//
    
    fillScreen(RA8875_WHITE);    
    CyDelay(myDelay);
    fillScreen(RA8875_RED);
    CyDelay(myDelay);
    fillScreen(RA8875_YELLOW);
    CyDelay(myDelay);
    fillScreen(RA8875_GREEN);
    CyDelay(myDelay);
    fillScreen(RA8875_BLUE);
    CyDelay(myDelay);
    fillScreen(RA8875_CYAN);
    CyDelay(myDelay);
    fillScreen(RA8875_MAGENTA);
    CyDelay(myDelay);
    fillScreen(RA8875_BLACK);
    
    //CyDelay(300);
    
    //----- Drawing Demo -----//
    /*
    drawCircle(100, 100, 50, RA8875_BLACK);
    fillCircle(100, 100, 49, RA8875_GREEN);
    fillRect(11, 11, 398, 198, RA8875_BLUE);
    drawRect(10, 10, 400, 200, RA8875_GREEN);
    drawPixel(10,10,RA8875_BLACK);
    drawPixel(11,11,RA8875_BLACK);
    drawLine(10, 10, 200, 100, RA8875_RED);
    drawTriangle(200, 15, 250, 100, 150, 125, RA8875_BLACK);
    fillTriangle(200, 16, 249, 99, 151, 124, RA8875_YELLOW);
    drawEllipse(300, 100, 100, 40, RA8875_BLACK);
    fillEllipse(300, 100, 98, 38, RA8875_GREEN);
    drawCurve(50, 100, 80, 40, 2, RA8875_BLACK);  
    fillCurve(50, 100, 78, 38, 2, RA8875_WHITE);
    */
        
    //--- Welcome Screen ---//
    /*
    CyDelay(300);
    PWM1out(0);
    fillScreen(RA8875_WHITE);
    
    for (int i = 0; i < 256; i++) {
        PWM1out(i);
        CyDelay(1);
    }

    textMode();
    fillRect(75,85,370,100,RA8875_RED);
    textSetCursor(100, 100);
    textEnlarge(1);
    textTransparent(RA8875_WHITE);
    textWrite(welcome1,strlen(welcome1)); 
    
    textEnlarge(0);
    textSetCursor(100, 200);
    textTransparent(RA8875_BLACK);
    textWrite(welcome2,strlen(welcome2)); 
    textSetCursor(100, 250);
    textWrite(welcome3,strlen(welcome3)); 
    
    textSetCursor(100, 350);
    textTransparent(RA8875_BLUE);
    textWrite(ble,strlen(ble));
    
    CyDelay(10000);
    */
}

void LCDTouchscreenDemo() {
    //------ Declarations ------//
    uint16_t tx;
    uint16_t ty;
    bool waiting = true;
    
    float xScale;
    float yScale;
    
    touchEnable(true);
    
    xScale = 1024.0F/width();
    yScale = 1024.0F/height();
    
    touchRead(&tx, &ty);

    
    //------ Wait for touch ------//
    //CyDelay(10);
    while ( waiting )  {
        if (!LCD_int_Read()) {
            //LED_BLUE_Write(0);
            if ( touched() ) {
                //LED_GREEN_Write(0u);
                touchRead(&tx, &ty);                
                fillCircle((uint16_t)(tx/xScale), (uint16_t)(ty/yScale), 4, RA8875_WHITE);
            }
//            else {
//                /* Clear TP INT Status */
//                //writeReg(RA8875_INTC2, RA8875_INTC2_TP);
//                touchRead(&tx, &ty);
//                CyDelay(1);
//                //tx = 0x0000;
//                //ty = 0x0000;
//            }
        }
//        else {
//            touchRead(&tx, &ty);
//            CyDelay(1);
//            //tx = 0x0000;
//            //ty = 0x0000;
//        }
    }
}

void GUIInit() {
    
    //------ Declarations ------/
    // LCD Strings
    char temp[] = "113";
    
    //------ Procedure ------//
    
    // Screen start-up
    PWM1out(0);
    fillScreen(RA8875_WHITE);
    for (int i = 0; i < 256; i++) {
        PWM1out(i);
        CyDelay(1);
    }
    
    // Print Temp
    textMode();
    textSetCursor(100, 100);
    textTransparent(RA8875_BLUE); 
//    PrintInt(255);
//    textSetCursor(100, 200);
}

void PrintInt(uint8_t n) {
    // Declarations
    uint8_t d;
    char p[] = "0";
    
    // Separate LSB
    d = n % 10;
    
    // Convert to ASCII (+48)
    p[0] = (d + 48);
    
    // Any digits to the left?
    if (n / 10 != 0) {
        PrintInt(n/10);
    }
    
    textWrite(p,strlen(p));
    return;
}

void PrintFloat(float n) {
    
    char dot[] = ".";
    
    // Extract integer part
    int ipart = (int) n;
    
    // Extract floating part
    float fpart = n - (float) ipart;
    
    // Convert integer part to string
    PrintInt(ipart);
    
    // Display 1 digit after decimal point
    textWrite(dot,strlen(dot));
    
    // Get the value of the fraction part up to 1 digit
    // after the dot.
    fpart = fpart * pow(10,1);
    
    PrintInt((int) fpart);    
}

void RegReadTest() {
    uint32 size_result = LCDSPI_SpiUartGetRxBufferSize();
    uint32 reg_result = readReg(0x22);
    
    if (size_result > 0) {
        LED_BLUE_Write(0);
    }
    
    if (reg_result != 0x0000) {
        LED_GREEN_Write(0);    
    }
    else {
        LED_GREEN_Write(1);
    }
    
}

void BLEConnect() {
    
     //------ Declarations ------/
    // LCD Strings
    char ok[] = "CYBLE_ERROR_OK";
    char error[] = "CYBLE_ERROR_OCCURED";
    char param[] = "CYBLE_ERROR_INVALID_PARAMETER";
    char operation[] = "CYBLE_ERROR_INVALID_OPERATION";
    char mem[] = "CYBLE_ERROR_MEMORY_ALLOCATION_FAILED";
    char state[] = "CYBLE_ERROR_INVALID_STATE";
    
    CYBLE_API_RESULT_T                      apiResult;
    
//    CYBLE_CONN_HANDLE_T                     connHandle;
    CYBLE_GATT_ATTR_HANDLE_RANGE_T          range;
    CYBLE_UUID_T                            uuid;
    CYBLE_GATTC_READ_BY_TYPE_REQ_T          readByTypeReqParam;
    
    CYBLE_API_RESULT_T scan_result;
    CYBLE_API_RESULT_T connect_result;
    CYBLE_STATE_T ble_state;
    
    //uint8 peripheral_address[6] = "000001";
    //uint8 peripheral_add_type = 0;
   // CYBLE_GAP_BD_ADDR_T peripheral_addr = {{0x00,0xA0,0x50,0x00,0x00,0x01}, 0};
    CYBLE_GAP_BD_ADDR_T peripheral_addr = {{0x01,0x00,0x00,0x50,0xA0,0x00}, 0};
    
    //------ GAP -------//
    
    CyBle_ProcessEvents();
    
    scan_result = CyBle_GapcStartScan( CYBLE_SCANNING_FAST );
    if (scan_result == CYBLE_ERROR_OK) {
          LED_GREEN_Write(0);  
    }
    CyBle_ProcessEvents();
    CyBle_GapcStopScan();
    CyBle_ProcessEvents();
//    if (DevicesNearBy == 0) {
//        LED_BLUE_Write(0);   
//    }
//    CyDelay(1000);
    connect_result = CyBle_GapcConnectDevice( &peripheral_addr );
    
    CyBle_ProcessEvents();
    if (connect_result == CYBLE_ERROR_OK) {
        LED_BLUE_Write(0); 
        CyDelay(500);
        LED_BLUE_Write(1);
        CyDelay(500);
    }
    
    //------ GATT ------//
//    
//            range.startHandle   = 0x0001;
//            range.endHandle     = 0xFFFF;
//            uuid.uuid16         = 0x14BF; 
//   
//            readByTypeReqParam.range = range;
//            readByTypeReqParam.uuid = uuid;
//            readByTypeReqParam.uuidFormat = 0x01;
    
//    connHandle.bdHandle = CYBLE_GAP_MAX_BONDED_DEVICE;
//    connHandle.attId = 0;
    
//    apiResult = CyBle_GattcStartDiscovery(cyBle_connHandle);
//    
//     if ( apiResult == CYBLE_ERROR_OK )
//                textWrite(ok,strlen(ok));
//    else if ( apiResult == CYBLE_ERROR_INVALID_PARAMETER)
//        textWrite(param,strlen(param));                                             
//    else if ( apiResult == CYBLE_ERROR_INVALID_OPERATION) 
//        textWrite(operation,strlen(operation));
//    else if ( apiResult == CYBLE_ERROR_MEMORY_ALLOCATION_FAILED) 
//        textWrite(mem,strlen(mem)); 
//    else if ( apiResult == CYBLE_ERROR_INVALID_STATE)                       // ERRORING HERE!
//        textWrite(state,strlen(state)); 

    
    while(1) {
        ble_state = CyBle_GetState();
        if (ble_state == CYBLE_STATE_CONNECTED ) {
            LED_BLUE_Write(0);   
//                        apiResult = CyBle_GattcReadUsingCharacteristicUuid(cyBle_connHandle,&readByTypeReqParam);
//                    if ( apiResult == CYBLE_ERROR_OK )
//                textWrite(ok,strlen(ok));
//            else if ( apiResult == CYBLE_ERROR_INVALID_PARAMETER )
//                textWrite(param,strlen(param));
//            else if ( apiResult == CYBLE_ERROR_INVALID_OPERATION )
//                textWrite(operation,strlen(operation));
        }
        else {
            LED_BLUE_Write(1);
        }
            
            
            //textWrite(serviceCount,strlen(serviceCount));
            //PrintInt(CYBLE_SRVI_COUNT);
            
            //apiResult = CyBle_GattcDiscoverAllCharacteristics(connHandle, range);
   

        CyBle_ProcessEvents();
//        CyDelay(1000);
    }
    
    
}

void FanTest() {
    while(1) {
        FAN1_Write(1);
        FAN2_Write(1);
        FAN3_Write(1);
        CyDelay(1000);
        FAN1_Write(0);
        FAN2_Write(0);
        FAN3_Write(0);
        CyDelay(1000);
    }
}
            
/* [] END OF FILE */
