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
#include <RA8875.h>
#include <math.h>

/*  Declarations    */
typedef struct Bin {
    float32 actual_temp;
    uint8_t set_temp;
    bool    fan_on;
    bool    ble_connected;
} Bin;

struct Outside {
    float32 temp;
} outside;

typedef enum Mode {
    MANUAL,
    AUTOMATIC
} Mode;

typedef enum Page {
    HOME,
    INFO
} Page;

struct Settings {
    Mode    mode;
    Page    page;
} settings;

void DefaultValues();
void StackEventHandler( uint32 eventCode, void *eventParam );
void LCDInit(void);
void LCDInitDemo();
void LCDTouchscreenDemo();
void LCDCheckTouch();
void GUISplashscreen();
void GUIHomeInit();
void GUIInfo();
void GUIUpdatePage();
void GUIUpdateBin1ActualTemp();
void GUIUpdateOutsideTemp();
void GUIUpdateMode();
void GUIUpdateControls();
void GUITouchHandler(uint16_t *tx, uint16_t *ty);
void GUITest();
void TempReceive(Bin * bin, uint8_t * fl_bytes);
void TempOutsideReceive(uint8_t * fl_bytes);
void PrintInt(uint8_t n);
void PrintFloat(float n);
void RegReadTest();
void BLEConnect();
void BLEIndicator();
void FanTest();
void FanUpdate();
bool FanValidCondition();

/* define the test register to switch the PA/LNA hardware control pins */
#define CYREG_SRSS_TST_DDFT_CTRL        0x40030008

/* Global Variables */
#define CYBLE_MAX_ADV_DEVICES           255

CYBLE_STATE_T                           ble_state;
CYBLE_GAP_BD_ADDR_T                     peerAddr[CYBLE_MAX_ADV_DEVICES];
uint8                                   DevicesNearBy = 0;
uint8_t                                 fl_temp_bytes[4];
uint8_t                                 byte = 0;
Bin                                     bin1;

int main()
{
    CyGlobalIntEnable;   /* Enable global interrupts */
    
    // Initialize Variables
    DefaultValues();
    
    // Power LED
    LED_RED_Write(0);
    
    /*  LCD Setup   */
    LCDInit();
    GUISplashscreen();
    GUIHomeInit();
    
    /* BLE Setup    */
    CyBle_Start( StackEventHandler );
    BLEConnect();
    
    for(;;)
    {
        ble_state = CyBle_GetState();
        if (ble_state == CYBLE_STATE_DISCONNECTED ) {
            BLEConnect();
        }
        LCDCheckTouch();
        FanUpdate();
        BLEIndicator();
        CyBle_ProcessEvents();
    }
}

void DefaultValues() {
    
    // Settings
    settings.mode = MANUAL;
    settings.page = HOME;
    
    // Outside
    outside.temp = 0;
    
    // Bin1
    bin1.actual_temp = 0;
    bin1.set_temp = 70;
    bin1.fan_on = false;
    bin1.ble_connected = false;
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
    CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T    notificationData;
    CYBLE_GATT_ATTR_HANDLE_RANGE_T          range;
    CYBLE_UUID_T                            uuid;
    CYBLE_API_RESULT_T                      apiResult;
   
    uint8                                   i;
    uint8_t                                 fl_bytes[4];
    
    
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
             //connHandle = *(CYBLE_CONN_HANDLE_T *) eventParam;
            
         break;
            
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
                /*Start to dicovery the servioes of the serve after connection*/
                CyBle_GattcStartDiscovery(cyBle_connHandle);  
        break;
            
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
           // printf("\r\nDiscovery complete.\r\n");
            
//            range.startHandle   = 0x0001;
//            range.endHandle     = 0xFFFF;
//            uuid.uuid16         = 0x14BF; 
//   
//            readByTypeReqParam.range = range;
//            readByTypeReqParam.uuid = uuid;
//            readByTypeReqParam.uuidFormat = 0x01;
            
            //textWrite(serviceCount,strlen(serviceCount));
            //PrintInt(CYBLE_SRVI_COUNT);
            
            //apiResult = CyBle_GattcDiscoverAllCharacteristics(connHandle, range);
   
            //apiResult = CyBle_GattcReadUsingCharacteristicUuid(cyBle_connHandle,&readByTypeReqParam);
            
//            if ( apiResult == CYBLE_ERROR_OK )
//                textWrite(ok,strlen(ok));

        break;
        
        case CYBLE_EVT_GATTC_READ_BY_TYPE_RSP:
            
            readResponse = *(CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T *) eventParam;
            
            //textWrite(dataLength,strlen(dataLength));
            //PrintInt(readResponse.attrData.length - 2);
            
            for( i=2 ; i < readResponse.attrData.length; i++)
            {
                //PrintInt(readResponse.attrData.attrValue[i]);
                //textWrite(space,strlen(space));
                
                fl_bytes[i-2] = readResponse.attrData.attrValue[i];
            }
            
//            if (notificationData.handleValPair.attrHandle == 0x0012) {
//                TempReceive(&bin1,fl_bytes);
//            }
//            else if (notificationData.handleValPair.attrHandle == 0x0016) {
//                TempOutsideReceive(fl_bytes);
//            }
            
        break;
                
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
                
            //textWrite(notification,strlen(notification));
            
            notificationData = *(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *) eventParam;
            
            fl_temp_bytes[byte++] = * notificationData.handleValPair.value.val;

            // convert 4 bytes to float
            if (byte == 4) {
                //PrintFloat( *(float32*)(fl_temp_bytes) );
                //textWrite(space,strlen(space));  
                
                if (notificationData.handleValPair.attrHandle == 0x0012) {
                    TempReceive(&bin1,fl_temp_bytes);
                }
                else if (notificationData.handleValPair.attrHandle == 0x0016) {
                    TempOutsideReceive(fl_temp_bytes);
                }
                byte = 0;
            }
                
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
    
    touchEnable(true);
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
    //textMode();
    //graphicsMode();
    
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

void LCDCheckTouch() {
    
    uint16_t tx;
    uint16_t ty;
    float xScale = 1024.0F/width();
    float yScale = 1024.0F/height();
    

    //touchEnable(true);
    //touchRead(&tx, &ty);
    if (!LCD_int_Read()) {
        if (touched()) {
            touchRead(&tx,&ty);
            //fillCircle((uint16_t)(tx/xScale), (uint16_t)(ty/yScale), 4, RA8875_WHITE);
            GUITouchHandler(&tx,&ty);
            while (!LCD_int_Read()) {
                // Sensing multiple touches
                // Most cases this doesn't matter, except for the set temp up/down buttons
                touchRead(&tx,&ty); 
            }
        }
    }
}

void GUISplashscreen() {
    
    //------ Declarations ------//
    // LCD Strings
    char welcome1[] = "Seed Bin Climate";
    char welcome2[] = "Control System";
    char welcome3[] = "Goesch Agronomy Services, Inc.";
    char welcome4[] = "v1";
    
    //------ Procedure ------//
    
    // Screen Fade-In
    PWM1out(0);
    fillScreen(RA8875_WHITE);
    for (int i = 0; i < 256; i++) {
        PWM1out(i);
        CyDelay(1);
    }
    
    // Company Info
    textMode();
    fillRect(75,85,650,175,RA8875_BLUE);
    
    textEnlarge(3);
    textTransparent(RA8875_WHITE);
    // Seed Bin Climate
    textSetCursor(100, 100);
    textWrite(welcome1,strlen(welcome1));
    // Control System
    textSetCursor(100, 175);
    textWrite(welcome2,strlen(welcome2));
    // Goesch Agronomy Services, Inc
    textEnlarge(2);
    textTransparent(RA8875_GREEN);
    textSetCursor(75, 325);
    textWrite(welcome3,strlen(welcome3));
    // v1
    textEnlarge(1);
    textTransparent(RA8875_BLACK);
    textSetCursor(750, 430);
    textWrite(welcome4,strlen(welcome4));
    
    CyDelay(3000);
}

void GUIHomeInit() {
    
    //------ Declarations ------//
    // LCD Strings
    char banner[] = "Seed Bin Climate Control System";
    char modes[] = "Modes";
    char manual[] = "Manual";
    char automatic[] = "Auto";
    char info[] = "Info";
    char outside[] = "Outside";
    char binno1[] = "Bin #1";
    
    int scr_width = 800;
    int scr_height = 480;
    
    //------ Procedure ------//
    
    // Background
    PWM1out(255);
    fillScreen(RA8875_WHITE);
    textMode();
    
    // Banner
    fillRect(20,20,
            scr_width-40,50,
            RA8875_GREEN);
    textEnlarge(2);
    textTransparent(RA8875_BLACK);
    textSetCursor(25,18);
    textWrite(banner,strlen(banner));
    
    // Modes
    fillRect(20,70,
            scr_width/3-40,scr_height/2+50,
            RA8875_BLACK);
    textEnlarge(3);
    textTransparent(RA8875_WHITE);
    textSetCursor(50,70);
    textWrite(modes,strlen(modes));
    
    GUIUpdateMode();
    textMode();
    
    // Info
    fillRect(20,scr_height/2+120,
            scr_width/3-40,scr_height/2-140,
            RA8875_BLUE);
    textEnlarge(3);
    textTransparent(RA8875_WHITE);
    textSetCursor(60,scr_height/2+135);
    textWrite(info,strlen(info));
    
    // Outside
    fillRect(scr_width/3-20,70,
            scr_width/3,scr_height/2+50,
            RA8875_CYAN);
    textEnlarge(3);
    textTransparent(RA8875_BLACK);
    textSetCursor(scr_width/3,70);
    textWrite(outside,strlen(outside));
    
    GUIUpdateOutsideTemp();
    textMode();
    
    // Bin1
    fillRect(2*scr_width/3-20,70,
            scr_width/3,scr_height/2+50,
            RA8875_RED);
    textEnlarge(3);
    textTransparent(RA8875_BLACK);
    textSetCursor(2*scr_width/3+20,70);
    textWrite(binno1,strlen(binno1));
    
    GUIUpdateBin1ActualTemp();
    textMode();
    
    // Controls
    fillRect(scr_width/3-20,scr_height/2+120,
            2*scr_width/3,scr_height/2-140,
            RA8875_YELLOW);    

    GUIUpdateControls();
    
    // Enable touch
    graphicsMode();
}

void GUIInfo() {
    
    //------ Declarations ------//
    // LCD Strings
    char banner[] = "Seed Bin Climate Control System";
    char designed[] = "Designed for GAS by:";
    char team9[] = "UNL ECE Capstone Team 9, 2018";
    char names1[] = "Megan Bird, Kyle Brozek";
    char names2[] = "Nick Jaton & Thallon Pitchure";
    char manual[] = "Manual Mode:";
    char manual1[] = "This mode displays the bin and outdoor";
    char manual2[] = "temperatures, but allows the user to control";
    char manual3[] = "the fan by pressing the ON and OFF buttons.";
    char automatic[] = "Auto Mode:";
    char automatic1[] = "This mode controls the fans based on the";
    char automatic2[] = "set temperature, with a 5 degree buffer";
    char automatic3[] = "before the fans are triggered on.";
    char home[] = "Home";
    
    int scr_width = 800;
    int scr_height = 480;
    
    //------ Procedure ------//
    
    // Background
    PWM1out(255);
    fillScreen(RA8875_BLUE);
    textMode();
    
    // Banner
    textEnlarge(2);
    textTransparent(RA8875_WHITE);
    textSetCursor(25,18);
    textWrite(banner,strlen(banner));
    
    // About
    textEnlarge(1);
    textSetCursor(25,100);
    textWrite(designed,strlen(designed));
    textEnlarge(0);
    textSetCursor(50,160);
    textWrite(team9,strlen(team9));
    textSetCursor(50,200);
    textWrite(names1,strlen(names1));
    textSetCursor(50,240);
    textWrite(names2,strlen(names2));    
    
    // Manual Mode
    textEnlarge(1);
    textSetCursor(400,100);
    textWrite(manual,strlen(manual));
    textEnlarge(0);
    textSetCursor(425,160);
    textWrite(manual1,strlen(manual1));
    textSetCursor(425,200);
    textWrite(manual2,strlen(manual2));
    textSetCursor(425,240);
    textWrite(manual3,strlen(manual3));   
    
    // Auto Mode
    textEnlarge(1);
    textSetCursor(400,280);
    textWrite(automatic,strlen(automatic));
    textEnlarge(0);
    textSetCursor(425,340);
    textWrite(automatic1,strlen(automatic1));
    textSetCursor(425,380);
    textWrite(automatic2,strlen(automatic2));
    textSetCursor(425,420);
    textWrite(automatic3,strlen(automatic3));
    
    // Home
    fillRect(20,scr_height/2+120,
            scr_width/3-40,scr_height/2-140,
            RA8875_WHITE);
    textEnlarge(3);
    textTransparent(RA8875_BLUE);
    textSetCursor(60,scr_height/2+135);
    textWrite(home,strlen(home));
    
    // Enable touch
    graphicsMode();
}

void GUIUpdatePage() {
    
    if (settings.page == HOME) {
        GUIInfo();
        settings.page = INFO;
    }
    else if (settings.page == INFO) {
        GUIHomeInit();
        settings.page = HOME;
    }
}

void GUIUpdateMode() {
    
    // LCD Strings
    char modes[] = "Modes";
    char manual[] = "Manual";
    char automatic[] = "Auto";
    
    int scr_width = 800;
    int scr_height = 480;
    
    textMode();
//    fillRect(20,70,
//            scr_width/3-40,scr_height/2+50,
//            RA8875_BLACK);
//    textEnlarge(3);
//    textTransparent(RA8875_WHITE);
//    textSetCursor(50,70);
//    textWrite(modes,strlen(modes));
    
    fillRect(20,150,
            scr_width/3-40,200,
            RA8875_BLACK);
    
    if (settings.mode == MANUAL) {
        fillRect(40,170,
            180,60,
            RA8875_WHITE);
        textEnlarge(2);
        textTransparent(RA8875_BLACK);
        textSetCursor(60,175);
        textWrite(manual,strlen(manual));
        drawRect(40,270,
                180,60,
                RA8875_WHITE);
        textTransparent(RA8875_WHITE);
        textSetCursor(80,275);
        textWrite(automatic,strlen(automatic));
    }  
    else if (settings.mode == AUTOMATIC) {
        drawRect(40,170,
            180,60,
            RA8875_WHITE);
        textEnlarge(2);
        textTransparent(RA8875_WHITE);
        textSetCursor(60,175);
        textWrite(manual,strlen(manual));
        fillRect(40,270,
                180,60,
                RA8875_WHITE);
        textTransparent(RA8875_BLACK);
        textSetCursor(80,275);
        textWrite(automatic,strlen(automatic));
    }
    GUIUpdateControls();
    graphicsMode();
}

void GUIUpdateBin1ActualTemp() {
    
    char space[] = " ";
    int scr_width = 800;
    
    
    if (settings.page == HOME) {
        textMode();
        textEnlarge(4);
        textColor(RA8875_BLACK,RA8875_RED);
        textSetCursor(2*scr_width/3+50,170);
        PrintFloat(bin1.actual_temp);
        textWrite(space,strlen(space));
        graphicsMode();
    }
}

void GUIUpdateOutsideTemp() {
    
    char space[] = " ";
    int scr_width = 800;

    if (settings.page == HOME) {
        textMode();
        textEnlarge(4);
        textColor(RA8875_BLACK,RA8875_CYAN);
        textSetCursor(scr_width/3+50,170);
        PrintFloat(outside.temp);
        textWrite(space,strlen(space));
        graphicsMode();
    }
}

void GUIUpdateControls() {
    
    // LCD Strings
    char on[] = "ON";
    char off[] = "OFF";
    char fans[] = "Fans";
    char set[] = "Set";
    
    int scr_width = 800;
    int scr_height = 480;
    
    
    textMode();
    
    // Controls
    fillRect(scr_width/3-20,scr_height/2+120,
            2*scr_width/3,scr_height/2-140,
            RA8875_YELLOW); 
    
    if (settings.mode == MANUAL) {
        textEnlarge(3);
        textTransparent(RA8875_BLACK);
        textSetCursor(scr_width/3+10,scr_height/2+135);
        textWrite(fans,strlen(fans));
        if (bin1.fan_on == false) {
            fillRect(scr_width/3+180,scr_height/2+135,
                    120,70,
                    RA8875_BLACK);
            textEnlarge(3);
            textTransparent(RA8875_YELLOW);
            textSetCursor(scr_width/3+195,scr_height/2+135);
            textWrite(off,strlen(off));
            
            drawRect(2*scr_width/3+80,scr_height/2+135,
                    120,70,
                    RA8875_BLACK);
            textEnlarge(3);
            textTransparent(RA8875_BLACK);
            textSetCursor(2*scr_width/3+110,scr_height/2+135);
            textWrite(on,strlen(on));
        } else {
            drawRect(scr_width/3+180,scr_height/2+135,
                    120,70,
                    RA8875_BLACK);
            textEnlarge(3);
            textTransparent(RA8875_BLACK);
            textSetCursor(scr_width/3+195,scr_height/2+135);
            textWrite(off,strlen(off));
            
            fillRect(2*scr_width/3+80,scr_height/2+135,
                    120,70,
                    RA8875_BLACK);
            textEnlarge(3);
            textTransparent(RA8875_YELLOW);
            textSetCursor(2*scr_width/3+110,scr_height/2+135);
            textWrite(on,strlen(on));
        }        
    }
    else if (settings.mode == AUTOMATIC) {
        textEnlarge(3);
        textTransparent(RA8875_BLACK);
        textSetCursor(scr_width/3+10,scr_height/2+135);
        textWrite(set,strlen(set));
        
        textSetCursor(scr_width/3+250,scr_height/2+135);
        PrintFloat(bin1.set_temp);
        
        drawRect(scr_width/3+160,scr_height/2+135,
                    70,70,
                    RA8875_BLACK);
        fillTriangle(scr_width/3+195,scr_height/2+185,
                    scr_width/3+215,scr_height/2+155,
                    scr_width/3+175,scr_height/2+155,
                    RA8875_BLACK);
        
        drawRect(scr_width/3+400,scr_height/2+135,
                    70,70,
                    RA8875_BLACK);
        fillTriangle(scr_width/3+435,scr_height/2+155,
                    scr_width/3+455,scr_height/2+185,
                    scr_width/3+415,scr_height/2+185,
                    RA8875_BLACK);
    }
    graphicsMode();
}

void GUITouchHandler(uint16_t *tx, uint16_t *ty) {
    
    float xScale = 1024.0F/width();
    float yScale = 1024.0F/height();
    
    uint16_t x = *tx/xScale;
    uint16_t y = *ty/yScale;
    
    int scr_width = 800;
    int scr_height = 480;
    
    
    // MODES
    // Manual button
    if (x > 40 && x < 170 && y > 170 && y < 230) {
        if (settings.mode != MANUAL) {
            settings.mode = MANUAL;
            GUIUpdateMode();
        }
    }
    // Auto button
    else if (x > 40 && x < 270 && y > 170 && y < 330) {
        if (settings.mode != AUTOMATIC) {
            settings.mode = AUTOMATIC;
            GUIUpdateMode();
        }
    }    
    
    // FAN
    // Off
    else if (settings.mode == MANUAL && x > scr_width/3+180 && x < scr_width/3+300 && y > scr_height/2+135 && y < scr_height/2+205) {
        if (bin1.fan_on != false) {
            bin1.fan_on = false;
            GUIUpdateControls();
        }
    }
    // On
    else if (settings.mode == MANUAL && x > 2*scr_width/3+80 && x < 2*scr_width/3+200 && y > scr_height/2+135 && y < scr_height/2+205) {
        if (bin1.fan_on != true) {
            bin1.fan_on = true;
            GUIUpdateControls();
        }
    }
    
    // SET TEMP
    // Down
    else if (settings.mode == AUTOMATIC && x > scr_width/3+160 && x < scr_width/3+230 && y > scr_height/2+135 && y < scr_height/2+205) {
        bin1.set_temp--;
        GUIUpdateControls();
    }
    // Up
    else if (settings.mode == AUTOMATIC && x > scr_width/3+400 && x < scr_width/3+470 && y > scr_height/2+135 && y < scr_height/2+205) {
        bin1.set_temp++;
        GUIUpdateControls();
    }
    
    // Info/Home
    else if (x > 20 && x < scr_width/3-20 && y > scr_height/2+120 && y < scr_height-20) {
        GUIUpdatePage();
    }
}

void GUITest() {
    
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

void TempReceive(Bin * bin, uint8_t * fl_bytes) {
    
    char space[] = " ";
    
    
    // Store value
    bin->actual_temp = *(float32*)(fl_bytes);
    
    // Update interface
    GUIUpdateBin1ActualTemp();
}

void TempOutsideReceive(uint8_t * fl_bytes) {
    
    char space[] = " ";
    
    
    // Store value
    outside.temp = *(float32*)(fl_bytes);
    
    // Update interface
    GUIUpdateOutsideTemp();
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
    
    char negative[] = "-";
    char dot[] = ".";
    
    
    // Extract integer part
    int ipart = (int) n;
    
    // Extract floating part
    float fpart = n - (float) ipart;
    
    // Convert integer part to string
    if (ipart < 0) {
        textWrite(negative,strlen(negative));
        ipart *= -1;
    }
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

    
    //while(1) {
        ble_state = CyBle_GetState();
        CyBle_ProcessEvents();
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
    //}
    
    
}

void BLEIndicator() {
    
    // LCD Strings
    char connected[] = "Connected";
    char disconnected[] = "Disconnected";
    
    int scr_width = 800;
    int scr_height = 480;
    
    CYBLE_STATE_T ble_state;
    
    
    ble_state = CyBle_GetState();
    if (settings.page == HOME) {
        textMode();
        if (ble_state == CYBLE_STATE_CONNECTED ) {
            LED_BLUE_Write(0);
            
            if (!bin1.ble_connected) {
                fillRect(2*scr_width/3,260,
                        scr_width/3-50,80,
                        RA8875_RED);
                
                textEnlarge(1);
                textTransparent(RA8875_BLACK);
                textSetCursor(2*scr_width/3+40,260);
                textWrite(connected,strlen(connected));
                bin1.ble_connected = true;
            }
        }
        else {
            LED_BLUE_Write(1);
            
            if (bin1.ble_connected) {
                fillRect(2*scr_width/3,260,
                        scr_width/3-50,80,
                        RA8875_RED);
                
                textEnlarge(1);
                textTransparent(RA8875_BLACK);
                textSetCursor(2*scr_width/3+20,260);
                textWrite(disconnected,strlen(disconnected));
                bin1.ble_connected = false;
            }
        }
        graphicsMode();
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

void FanUpdate() {
    
    uint8_t buffer = 5;
    
    // Auto Mode:  Update fan state
    if (settings.mode == AUTOMATIC) {
        if (!bin1.fan_on) {
            // Must be outside buffer zone to turn on
            if (bin1.actual_temp < bin1.set_temp-buffer || bin1.actual_temp > bin1.set_temp+buffer) {
                // Then must meet valid fan running conditions
                bin1.fan_on = FanValidCondition();
            }
        }
        else {
            bin1.fan_on = FanValidCondition();
        }
    }
    
    if (bin1.fan_on == true) {
        FAN1_Write(1);
    } else {
        FAN1_Write(0);
    }
}

bool FanValidCondition() {
    
    if (bin1.set_temp < bin1.actual_temp && outside.temp < bin1.actual_temp) return true;
    else if (bin1.set_temp > bin1.actual_temp && outside.temp > bin1.actual_temp) return true;
    else return false;
}
            
/* [] END OF FILE */
