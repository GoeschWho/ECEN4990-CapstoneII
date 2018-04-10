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
#include "DS18B20.h"

// Pin drive modes
#define     OUT     1
#define     IN      0

// OneWire commands
#define STARTCONVO      0x44  // Tells device to take a temperature reading and put it on the scratchpad
#define COPYSCRATCH     0x48  // Copy EEPROM
#define READSCRATCH     0xBE  // Read EEPROM
#define WRITESCRATCH    0x4E  // Write to EEPROM
#define RECALLSCRATCH   0xB8  // Reload from last known
#define READPOWERSUPPLY 0xB4  // Determine if device needs parasite power
#define ALARMSEARCH     0xEC  // Query bus for devices with an alarm condition

// Scratchpad locations
#define TEMP_LSB        0
#define TEMP_MSB        1
#define HIGH_ALARM_TEMP 2
#define LOW_ALARM_TEMP  3
#define CONFIGURATION   4
#define INTERNAL_BYTE   5
#define COUNT_REMAIN    6
#define COUNT_PER_C     7
#define SCRATCHPAD_CRC  8

// Device resolution
#define TEMP_9_BIT  0x1F //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

/* ------------------------------------------------------------
 * HIGH-LEVEL FUNCTIONS
 * ------------------------------------------------------------
*/

/* ------------------------------------------------------------
 * Function: Temp_Reset
 * ------------------------------------------------------------
 * Description: Performs the reset function.  Will wait up to 
 *              250us for the bus to come high.  If it doesn't, 
 *              then it isbroken or shorted and we return a 0.
 * 
 * Parameters:  Sensor number
 * 
 * Returns:     1 if a device asserted a presence pulse, 0 otherwise.
 * ------------------------------------------------------------
*/
uint8_t Temp_Reset(uint8_t n) {
    
    uint8_t r;
    uint8_t retries = 125;
    
    
    Temp_SetDriveMode(n,IN);
    do {
        if (--retries == 0) return 0;
        CyDelayUs(2);
    } while ( !Temp_ReadPin(n));
    
    Temp_WritePin(n,0);
    Temp_SetDriveMode(n,OUT);   // drive output low
    CyDelayUs(480);
    Temp_SetDriveMode(n,IN);    // allow to float
    CyDelayUs(70);
    r = !Temp_ReadPin(n);
    CyDelayUs(410);
    return r;
}

/* ------------------------------------------------------------
 * Function: Temp_ROMSelect
 * ------------------------------------------------------------
 * Description: Issues a match ROM command.
 * 
 * Parameters:  Sensor number, ROM address
 * 
 * Returns:     Nothing
 * ------------------------------------------------------------
*/
void Temp_ROMSelect(uint8_t n, const uint8_t rom[8]) {
    
    uint8_t i;
    
    
    Temp_Write(n,0x55);        // Choose ROM
    for (i = 0; i < 8; i++) Temp_Write(n,rom[i]);
}

/* ------------------------------------------------------------
 * Function: Temp_ROMSkip
 * ------------------------------------------------------------
 * Description: Issues a skip ROM command.
 * 
 * Parameters:  Sensor number
 * 
 * Returns:     Nothing
 * ------------------------------------------------------------
*/
void Temp_ROMSkip(uint8_t n) {
    
    Temp_Write(n,0xCC);     // Skip ROM
}

/* ------------------------------------------------------------
 * Function: Temp_RequestTemp
 * ------------------------------------------------------------
 * Description: Issues a begin conversion command.  The line
 *              will go low until the conversion is complete.
 * 
 * Parameters:  Sensor number
 * 
 * Returns:     Nothing
 * ------------------------------------------------------------
*/
void Temp_RequestTemp(uint8_t n) {
    
    Temp_Reset(n);
    Temp_ROMSkip(n);
    Temp_Write(n,STARTCONVO);
}



/* ------------------------------------------------------------
 * MID-LEVEL DRIVERS
 * ------------------------------------------------------------
*/

/* ------------------------------------------------------------
 * Function: Temp_Write
 * ------------------------------------------------------------
 * Description: Writes a byte to the selected sensor.  This does
 *              not use parasite power mode, pin goes tri-state
 *              at the end of the write.
 * 
 * Parameters:  Sensor number, value (0 or 1)
 * 
 * Returns:     Nothing
 * ------------------------------------------------------------
*/
void Temp_Write(uint8_t n, uint8_t byte) {
    
    uint8_t bitMask;
    
    
    for (bitMask = 0x01; bitMask; bitMask <<= 1) {
        Temp_WriteBit(n,(bitMask & byte)?1:0);
        Temp_SetDriveMode(n,IN);
        Temp_WritePin(n,0);
    }
}

/* ------------------------------------------------------------
 * Function: Temp_Read
 * ------------------------------------------------------------
 * Description: Reads a byte from the selected sensor.
 * 
 * Parameters:  Sensor number, value (0 or 1)
 * 
 * Returns:     Byte read
 * ------------------------------------------------------------
*/
uint8_t Temp_Read(uint8_t n) {
    
    uint8_t bitMask;    
    uint8_t r;
    
    
    for (bitMask = 0x01; bitMask; bitMask <<=1) {
        if (Temp_ReadBit(n)) r |= bitMask;
    }
    return r;
}

/* ------------------------------------------------------------
 * Function: Temp_WriteBit
 * ------------------------------------------------------------
 * Description: Writes a bit to the selected sensor.
 * 
 * Parameters:  Sensor number, value (0 or 1)
 * 
 * Returns:     Nothing
 * ------------------------------------------------------------
*/
void Temp_WriteBit(uint8_t n, uint8_t b) {
    
    if (b == 1) {
        Temp_WritePin(n,0);
        Temp_SetDriveMode(n,OUT);   // drive output low
        CyDelayUs(10);
        Temp_WritePin(n,1);         // drive output high
        CyDelayUs(55);
    } else {
        Temp_WritePin(n,0);
        Temp_SetDriveMode(n,OUT);   // drive output low
        CyDelayUs(65);
        Temp_WritePin(n,1);         // drive output high
        CyDelayUs(5);       
    }
}

/* ------------------------------------------------------------
 * Function: Temp_ReadBit
 * ------------------------------------------------------------
 * Description: Writes a bit to the selected sensor.
 * 
 * Parameters:  Sensor number, value (0 or 1)
 * 
 * Returns:     Nothing
 * ------------------------------------------------------------
*/
uint8_t Temp_ReadBit(uint8_t n) {
    
    uint8_t r;
    
    
    Temp_WritePin(n,0);
    Temp_SetDriveMode(n,OUT);
    CyDelayUs(3);
    Temp_SetDriveMode(n,IN);    // let pin float, pull-up will raise
    CyDelayUs(10); 
    r = Temp_ReadPin(n);
    CyDelayUs(53);
    return r;
}

/* ------------------------------------------------------------
 * LOW-LEVEL DRIVERS
 *
 * To be customized by user
 * ------------------------------------------------------------
*/

/* ------------------------------------------------------------
 * Function: Temp_SetDriveMode
 * ------------------------------------------------------------
 * Description: Sets the pin drive mode for the selected sensor.
 *              This function needs configured by the developer
 *              by setting the corresponding read functions for 
 *              their environment and pins.
 * 
 * Parameters:  Sensor number, drive mode (0-out, 1-in)
 * 
 * Returns:     Nothing
 * ------------------------------------------------------------
*/
void Temp_SetDriveMode(uint8_t n, uint8_t mode) {
    
    switch (n) {
        
        case 1:
            if (mode == IN) Temp1_SetDriveMode(Temp1_DM_DIG_HIZ);
            else Temp1_SetDriveMode(Temp1_DM_STRONG);
        break;
            
        case 2:
            if (mode == IN) Temp2_SetDriveMode(Temp2_DM_DIG_HIZ);
            else Temp2_SetDriveMode(Temp2_DM_STRONG);
        break;
            
        default:
        break;
    }
}

/* ------------------------------------------------------------
 * Function: Temp_ReadPin
 * ------------------------------------------------------------
 * Description: Returns the logic value on the selected sensor.
 *              This function needs configured by the developer
 *              by setting the corresponding read functions for 
 *              their environment and pins.
 * 
 * Parameters:  Sensor number
 * 
 * Returns:     Digital value of the pin (0 or 1), 
 *              or 0xFF if an invalid sensor was selected.
 * ------------------------------------------------------------
*/
uint8_t Temp_ReadPin(uint8_t n) {
    
    switch (n) {
        
        case 1:
            return Temp1_Read();
        break;
        
        case 2:
            return Temp2_Read();
        break;
        
        default:
            return 0xFF;
    }
}

/* ------------------------------------------------------------
 * Function: Temp_WritePin
 * ------------------------------------------------------------
 * Description: Writes the logic value to the selected sensor.
 *              This function needs configured by the developer
 *              by setting the corresponding read functions for 
 *              their environment and pins.
 * 
 * Parameters:  Sensor number, value to write (0 or 1)
 * 
 * Returns:     Nothing
 * ------------------------------------------------------------
*/
void Temp_WritePin(uint8_t n, uint8_t value) {
    
    switch (n) {
        
        case 1:
            if (value == 0) Temp1_Write(0);
            else if (value == 1) Temp1_Write(1);
        break;
        
        case 2:
            if (value == 0) Temp2_Write(0);
            else if (value == 1) Temp2_Write(1);
        break;
        
        default:
        break;
    }
}

/* [] END OF FILE */
