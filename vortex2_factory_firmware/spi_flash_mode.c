#include "mode_global.h"
#include "spi_flash_mode.h"
#include "pca10056.h"
#include "nrf_log.h"
#include "nrf_drv_spi.h"
#include "vortex2_flash.h"

#define   CS_PIN  10
#define   CLK_PIN 22
#define   DO_PIN  17
#define   DIO_PIN 20

#define   CS_L    nrf_gpio_pin_clear(CS_PIN)
#define   SCLK_L  nrf_gpio_pin_clear(CLK_PIN)
#define   DO_L    nrf_gpio_pin_clear(DO_PIN)
#define   DIO_L   nrf_gpio_pin_clear(DIO_PIN)

#define   CS_H    nrf_gpio_pin_set(CS_PIN)
#define   SCLK_H  nrf_gpio_pin_set(CLK_PIN)
#define   DO_H    nrf_gpio_pin_set(DO_PIN)
#define   DIO_H   nrf_gpio_pin_set(DIO_PIN)

#define   DO_Read 1

#define SPI_INSTANCE  1 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);
static volatile bool spi_xfer_done;
nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;

#define TEST_STRING "Nordic"
static uint8_t       m_tx_buf[] = {0x90,0x00,0x00,0x00,0xFF,0xFF};           /**< TX buffer. */
static uint8_t       m_rx_buf[sizeof(TEST_STRING) + 1];    /**< RX buffer. */
static const uint8_t m_length = sizeof(m_tx_buf);        /**< Transfer length. */
static uint8_t       tx_data[256];
static uint8_t       rx_data[256];
static uint8_t       tx_len = 0;
static uint8_t       rx_len = 0;

/*

NRF_SPI_Type *_p_spi;

static void spi_init(NRF_SPI_Type *p_spi, uint8_t pinMISO, uint8_t pinSCK, uint8_t pinMOSI)
{
  _p_spi = p_spi;
  _p_spi->PSELSCK  = pinSCK;
  _p_spi->PSELMOSI = pinMosi;
  _p_spi->PSELMISO = pinMiso;
}*/

static void spi_event_clear(NRF_SPI_Type * p_reg, nrf_spi_event_t spi_event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)spi_event)) = 0x0UL;
#if __CORTEX_M == 0x04
    volatile uint32_t dummy = *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)spi_event));
    (void)dummy;
#endif
}


static bool spi_event_check(NRF_SPI_Type * p_reg,nrf_spi_event_t spi_event)
{
  return (bool)*(volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)spi_event);
}


static void beginTransfer()
{
  CS_L;
  spi.u.spi.p_reg->INTENCLR = NRF_SPI_INT_READY_MASK;
  nrf_spi_event_clear(spi.u.spi.p_reg, NRF_SPI_EVENT_READY);
}

static void stopTransfer()
{
  spi.u.spi.p_reg->INTENSET = NRF_SPI_INT_READY_MASK;
  CS_H;
}
static uint8_t transfer(uint8_t data)
{
  spi.u.spi.p_reg->TXD = data;
  while (!spi_event_check(spi.u.spi.p_reg, NRF_SPI_EVENT_READY)) {}
  nrf_spi_event_clear(spi.u.spi.p_reg, NRF_SPI_EVENT_READY);
  data = spi.u.spi.p_reg->RXD;
  return data;
}


static void Write_Enable(void)
{
  beginTransfer();
  transfer(0x06);
  stopTransfer();
}

static void Write_Disable(void)
{
  beginTransfer();
  transfer(0x04);
  stopTransfer();
}

static uint8_t W25Q16_BUSY(void)
{
  uint8_t flag;
  beginTransfer();
  flag = transfer(0x05);
  stopTransfer();
  flag &= 0x01;
  return flag;
}

void W25Q16_Write(uint32_t address, uint8_t *data, uint32_t num)
{
  uint32_t i;
  while(W25Q16_BUSY());
  Write_Enable();
  beginTransfer();
  transfer(0x02);
  transfer(address >> 16);
  transfer(address >> 8);
  transfer(address );
  for(i = 0; i < num; i++){
    transfer(*(data+i));
  }
  stopTransfer();
}

void W25Q16_Read(uint32_t address, uint8_t *data, uint8_t num)
{
  uint8_t i;
  while(W25Q16_BUSY());
  beginTransfer();
  transfer(0x03);
  transfer(address >> 16);
  transfer(address >> 8);
  transfer(address );
  for(i = 0; i < num; i++){
    *(data + i) = transfer(0xFF);
  }
  stopTransfer();
}

void W25Q16_Erasure()
{
  Write_Enable();
  beginTransfer();
  transfer(0x00);
  stopTransfer();
  Write_Enable();
  while(W25Q16_BUSY());
  beginTransfer();
  transfer(0xc7);
  stopTransfer();
  while(W25Q16_BUSY());
}

void W25Q16_init()
{
    spi_config.ss_pin   = CS_PIN;
    spi_config.miso_pin = DO_PIN;
    spi_config.mosi_pin = DIO_PIN;
    spi_config.sck_pin  = CLK_PIN;
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, NULL, NULL));
    
    uint8_t data[16];
    uint8_t redata[16];
    for(uint8_t i = 0; i < 16; i++){
      data[i] = i;
    }
    NRF_LOG_INFO("begin Erasure");
    W25Q16_Erasure();
    NRF_LOG_INFO("done Erasure");
    /*
    while(1){
      W25Q16_Read(0x00,redata,16);
      for(uint8_t i = 0; i < 16; i++){
        NRF_LOG_INFO("redata[%d] = 0x%X\r\n",i,redata[i]);
      }
      vTaskDelay(1000);
    }*/
}


void Firmware_MoveTo_ExternFlash(){
    uint8_t data2[16]; 
    vortex2_flash_read_bytes(APP_VSTART,data2,16);
    for(uint8_t i = 0; i < 16; i++){
      NRF_LOG_INFO("data2[%d]=0x%X",i,data2[i]);
    }
    W25Q16_Write(0x00,data2,16);
    uint8_t data1[16];
    NRF_LOG_INFO("begin read");
    W25Q16_Read(0x00,data1,16);
    for(uint8_t i = 0; i < 16; i++){
      NRF_LOG_INFO("data1[%d]=0x%X",i,data1[i]);
    }
}





uint16_t W25QXX_TYPE = W25Q16;

void W25QXX_Init(void){
    spi_config.ss_pin   = CS_PIN;
    spi_config.miso_pin = DO_PIN;
    spi_config.mosi_pin = DIO_PIN;
    spi_config.sck_pin  = CLK_PIN;
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, NULL, NULL));
    CS_H;
    W25QXX_TYPE=W25QXX_ReadID();
}

uint16_t W25QXX_ReadID(void){
    uint16_t Temp = 0;
    beginTransfer();
    transfer(0x90);
    transfer(0x00);
    transfer(0x00);
    transfer(0x00);
    Temp |= transfer(0xFF)<<8;
    Temp |= transfer(0xFF);
    stopTransfer();
    return Temp;
}

uint8_t W25QXX_ReadSR(void){
    uint8_t byte = 0;
    beginTransfer();
    transfer(W25X_ReadStatusReg);
    byte = transfer(0xFF);
    stopTransfer();
    return byte;
}

void W25QXX_Write_SR(uint8_t sr){
    beginTransfer();
    transfer(W25X_WriteStatusReg);
    transfer(sr);
    stopTransfer();
}

void W25QXX_Write_Enable(void){
    beginTransfer();
    transfer(W25X_WriteEnable);
    stopTransfer();
}

void W25QXX_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead){
    uint16_t i;
    beginTransfer();
    transfer(W25X_ReadData);
    transfer((uint8_t)(ReadAddr >> 16));
    transfer((uint8_t)(ReadAddr >> 8));
    transfer((uint8_t)ReadAddr);
    for(i = 0; i < NumByteToRead; i++){
      pBuffer[i] = transfer(0xFF);
    }
    stopTransfer();
}

void W25QXX_Write_Page(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite){
    uint16_t i;
    W25QXX_Write_Enable();
    beginTransfer();
    transfer(W25X_PageProgram);
    transfer((uint8_t)(WriteAddr >> 16));
    transfer((uint8_t)(WriteAddr >> 8));
    transfer((uint8_t)(WriteAddr));
    for(i = 0; i < NumByteToWrite; i++){
      transfer(pBuffer[i]);
    }
    stopTransfer();
    W25QXX_Wait_Busy();
}

void W25QXX_Write_NoCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite){
    uint16_t pageremain;
    pageremain = 256 - WriteAddr%256;
    if(NumByteToWrite <= pageremain){
      pageremain = NumByteToWrite;
    }
    while(1){
      W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
      if(NumByteToWrite == pageremain){
        break;
      }else{
        pBuffer += pageremain;
        WriteAddr += pageremain;
        NumByteToWrite -= pageremain;
        if(NumByteToWrite > 256){
          pageremain = 256;
        }else{
          pageremain = NumByteToWrite;
        }
      }
    };  
}

static uint8_t W25QXX_BUFFER[4096];
void W25QXX_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite){
    uint32_t secpos;
    uint16_t secoff,secremain,i;
    uint8_t *W25QXX_BUF;
    W25QXX_BUF = W25QXX_BUFFER;
    secpos = WriteAddr/4096;
    secoff = WriteAddr%4096;
    secremain = 4096 - secoff;
    if(NumByteToWrite <= secremain){
      secremain = NumByteToWrite;
    }
    while(1){
      W25QXX_Read(W25QXX_BUF,secpos*4096,4096);
      for(i = 0; i < secremain; i++){
        if(W25QXX_BUF[secoff + i] != 0xFF)break;
      }
      if(i < secremain){
        W25QXX_Erase_Sector(secpos);
        for(i = 0; i < secremain; i++){
          W25QXX_BUF[i + secoff] = pBuffer[i];
        }
        W25QXX_Write_NoCheck(W25QXX_BUF, secpos*4096, 4096);
      }else{
        W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain);
      }

      if(NumByteToWrite == secremain){
        break;
      }else{
        secpos++;
        secoff = 0;
        pBuffer += secremain;
        WriteAddr += secremain;
        NumByteToWrite -= secremain;
        if(NumByteToWrite > 4096){
          secremain = 4096;
        }else{
          secremain = NumByteToWrite;
        }
      }
    };
}

void W25QXX_Erase_Chip(void){
    W25QXX_Write_Enable();
    W25QXX_Wait_Busy();
    beginTransfer();
    transfer(W25X_ChipErase);
    stopTransfer();
    W25QXX_Wait_Busy();
}

void W25QXX_Erase_Sector(uint32_t Dst_Addr){
    Dst_Addr *= 4096;
    W25QXX_Write_Enable();
    W25QXX_Wait_Busy();
    beginTransfer();
    transfer(W25X_SectorErase);
    transfer((uint8_t)(Dst_Addr >> 16));
    transfer((uint8_t)(Dst_Addr >> 8));
    transfer((uint8_t)(Dst_Addr));
    stopTransfer();
    W25QXX_Wait_Busy();
}

void W25QXX_Wait_Busy(void){
    while((W25QXX_ReadSR() & 0x01) == 0x01);
}

void W25QXX_PowerDowm(void){
    beginTransfer();
    transfer(W25X_PowerDown);
    stopTransfer();
}

void W25QXX_WAKEUP(void){
    beginTransfer();
    transfer(W25X_ReleasePowerDown);
    stopTransfer();
}