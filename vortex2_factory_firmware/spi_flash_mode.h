
#ifndef SPI_FLASH_MODE_H__
#define SPI_FLASH_MODE_H__
#include "stdint.h"

#define W25Q80   0xEF13
#define W25Q16   0xEF14
#define W25Q32   0xEF15
#define W25Q64   0xEF16
#define W25Q128  0xEF17
extern uint16_t W25QXX_TYPE;

#define   W25X_WriteEnable      0x06
#define   W25X_WriteDisable     0x04
#define   W25X_ReadStatusReg    0x05
#define   W25X_WriteStatusReg   0x01
#define   W25X_ReadData         0x03
#define   W25X_FastReadData     0x0B
#define   W25X_FastReadDual     0x3B
#define   W25X_PageProgram      0x02
#define   W25X_BlockErase       0xD8
#define   W25X_SectorErase      0x20
#define   W25X_ChipErase        0xC7
#define   W25X_PowerDown        0xB9
#define   W25X_ReleasePowerDown 0xAB
#define   W25X_DeviceID         0xAB
#define   W25X_ManufactDeviceID 0x90
#define   W25X_JedecDeviceID    0x9F


void      W25QXX_Init(void);
uint16_t  W25QXX_ReadID(void);
uint8_t   W25QXX_ReadSR(void);
void      W25QXX_Write_SR(uint8_t sr);
void      W25QXX_Write_Enable(void);
void      W25QXX_Write_Disable(void);
void      W25QXX_Write_NoCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void      W25QXX_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void      W25QXX_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void      W25QXX_Erase_Chip(void);
void      W25QXX_Erase_Sector(uint32_t Dst_Addr);
void      W25QXX_Wait_Busy(void);
void      W25QXX_PowerDown(void);
void      W25QXX_WAKEUP(void);



//void spi_flash_init(void);
void W25Q16_init(void);
void W25Q16_Erasure(void);
void W25Q16_Read(uint32_t address, uint8_t *data, uint8_t num);
void W25Q16_Write(uint32_t address, uint8_t *data, uint32_t num);
void Firmware_MoveTo_ExternFlash(void);
#endif 
