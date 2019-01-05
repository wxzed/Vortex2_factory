#include "mode_global.h"
#include "i2c_mode.h"
#include "pca10056.h"
#include "nrf_log.h"
#include "nrf_drv_twi.h"

#if TWI0_ENABLED
#define TWI_INSTANCE_ID     0
#elif TWI1_ENABLED
#define TWI_INSTANCE_ID     1
#endif

#define TWI_ADDRESSES      127
unsigned char rxbuf[10] = {0};

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

#define BUFFER_LENGTH 32
static uint8_t rxBuffer[BUFFER_LENGTH];
static uint16_t rxIndex;
static uint16_t rxLength;

static uint8_t txBuffer[BUFFER_LENGTH];
static uint16_t txIndex;
static uint16_t txLength;
static uint8_t txAddress;
static uint8_t transmitting;

void twi_init (void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_config = {
       .scl                = ARDUINO_SCL_PIN,
       .sda                = ARDUINO_SDA_PIN,
       .frequency          = NRF_DRV_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}


static void i2c_begin(){
    ret_code_t err_code;
    twi_init();
}
uint8_t i2c_requestFrom(uint8_t address, uint8_t size)
{
    if(size > BUFFER_LENGTH) {
        size = BUFFER_LENGTH;
    }
    uint8_t read = (nrf_drv_twi_rx(&m_twi, address, rxBuffer, size) == 0)?size:0;
    rxIndex = 0;
    rxLength = read;
    return read;
}
void i2c_beginTransmission(uint8_t address)
{
    transmitting = 1;
    txAddress = address;
    txIndex = 0;
    txLength = 0;
}

void i2c_endTransmission()
{
    uint8_t ret;
    if(txLength == 0){
	ret = nrf_drv_twi_tx(&m_twi, txAddress, txBuffer, 1, false);
    }else{
	ret = nrf_drv_twi_tx(&m_twi, txAddress, txBuffer, txLength, false);
    }
    txIndex = 0;
    txLength = 0;
    transmitting = 0;
}


void writebyte(uint8_t data)
{
    if(transmitting) {
        if(txLength >= BUFFER_LENGTH) {
        }
        txBuffer[txIndex] = data;
        ++txIndex;
        txLength = txIndex;
    }
}

static void write(uint8_t *data, uint8_t size)
{
    if(transmitting){
        for(size_t i = 0; i < size; ++i)
	{
            writebyte(data[i]);
	}
    }
}

uint8_t i2c_read()
{
    uint8_t value = -1;
    if(rxIndex < rxLength) {
        value = rxBuffer[rxIndex];
        ++rxIndex;
    }
    return value;
}


static uint8_t available(void)
{
    uint8_t result = rxLength - rxIndex;
    return result;
}

void i2cWriteBytes(uint8_t addr_t, uint8_t Reg , uint8_t *pdata, uint8_t datalen )
{
    i2c_beginTransmission(addr_t); 
    writebyte(Reg);             

    for (uint8_t i = 0; i < datalen; i++) {
      writebyte(*pdata);
      pdata++;
    }
    i2c_endTransmission();
}


uint16_t i2cReadBytes(uint8_t addr_t, uint8_t Reg ,uint8_t Num )
{
  unsigned char i = 0;
  i2c_beginTransmission(addr_t); // transmit to device #8
  writebyte(Reg);              // sends one byte
  i2c_endTransmission();    // stop transmitting
  i2c_requestFrom(addr_t, Num);
  while (available())   // slave may send less than requested
  {
    rxbuf[i] = i2c_read();
    i++;
  }
  return ((uint16_t)rxbuf[0] << 8) + rxbuf[1];
}