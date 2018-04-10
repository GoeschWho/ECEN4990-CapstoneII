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

//------ Protypes ------//

// High-Level Functions
uint8_t Temp_Reset(uint8_t n);
void Temp_ROMSelect(uint8_t n, const uint8_t rom[8]);
void Temp_ROMSkip(uint8_t n);
void Temp_RequestTemp(uint8_t n);

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
