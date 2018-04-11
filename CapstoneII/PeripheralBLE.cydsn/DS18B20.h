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

//------ Definitions ------//
typedef enum { false, true } bool;


//------ Protypes ------//

// High-Level Functions
uint8_t Temp_Reset(uint8_t n);
void Temp_ROMSelect(uint8_t n, const uint8_t rom[8]);
void Temp_ROMSkip(uint8_t n);
void Temp_RequestTemp(uint8_t n);
float32 Temp_GetTempF(uint8_t n);
bool Temp_ReadScratchPad(uint8_t n, uint8_t* scratchPad);
int16_t Temp_CalculateTemperature(uint8_t* scratchPad);
float32 Temp_RawToFahrenheit(uint16_t raw);
uint8_t Temp_CRC(uint8_t* scratchPad);

// Mid-Level Drivers
void Temp_Write(uint8_t n, uint8_t byte);
uint8_t Temp_Read(uint8_t n);
void Temp_WriteBit(uint8_t n, uint8_t b);
uint8_t Temp_ReadBit(uint8_t n);

// Low-Level Drivers
void Temp_SetDriveMode(uint8_t n, uint8_t mode);
uint8_t Temp_ReadPin(uint8_t n);
void Temp_WritePin(uint8_t n, uint8_t value);

/* [] END OF FILE */
