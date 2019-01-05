#include "ht1632c.h"
#include "pca10056.h"
#include "nrf_log.h"
#include "stdlib.h"

#define DATA_L      nrf_gpio_pin_clear(Display_DATA_Pin)
#define WR_L        nrf_gpio_pin_clear(Display_WR_Pin)
#define RD_L        nrf_gpio_pin_clear(Display_RD_Pin)
#define CS_L        nrf_gpio_pin_clear(Display_CS_Pin)

#define DATA_H      nrf_gpio_pin_clear(Display_DATA_Pin)
#define WR_H        nrf_gpio_pin_clear(Display_WR_Pin)
#define RD_H        nrf_gpio_pin_clear(Display_RD_Pin)
#define CS_H        nrf_gpio_pin_clear(Display_CS_Pin)
#define LOW         0
#define HIGH        1

static uint8_t _pinCS[4];
static uint8_t _numActivePins;
static uint8_t _pinWR;
static uint8_t _pinDATA;
static uint8_t _pinCLK;
static uint8_t _currSelectionMask;
static uint8_t _tgtRender;
static uint8_t _tgtChannel;
static uint8_t * mem [5];

static uint8_t const ht1632c_error_data[]={
1,1,0,0,0,0,1,1,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
1,0,0,0,0,0,0,1,
1,1,0,0,0,0,1,1,
0,1,1,0,0,1,1,0,
0,0,1,1,1,1,0,0,
0,0,0,1,1,0,0,0,
0,0,1,1,1,1,0,0,
0,1,1,0,0,1,1,0,
1,1,0,0,0,0,1,1,
1,0,0,0,0,0,0,1,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0
};


uint8_t const ht1632c_TurnLeft_data[]={
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  1,  1,  1,  1,  0,  0,
  0,  1,  1,  1,  1,  1,  1,  0,
  1,  1,  0,  1,  1,  0,  1,  1,
  1,  0,  0,  1,  1,  0,  0,  1,
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0
};

uint8_t const ht1632c_TurnRight_data[]={
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0,
  1,  0,  0,  1,  1,  0,  0,  1,
  1,  1,  0,  1,  1,  0,  1,  1,
  0,  1,  1,  1,  1,  1,  1,  0,
  0,  0,  1,  1,  1,  1,  0,  0,
  0,  0,  0,  1,  1,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0
};


uint8_t const ht1632c_Advance_data[]={
  0,  0,  0,  0,  0,  0,  0,  0,

  0,  0,  0,  0,  0,  0,  0,  0,

  0,  0,  0,  0,  0,  0,  0,  0,

  0,  0,  0,  0,  0,  0,  0,  0,

  0,  0,  0,  0,  1,  0,  0,  0,

  1,  0,  0,  0,  1,  1,  0,  0,

  1,  1,  0,  0,  0,  1,  1,  0,

  1,  1,  1,  1,  1,  1,  1,  1,

  1,  1,  1,  1,  1,  1,  1,  1,

  1,  1,  0,  0,  0,  1,  1,  0,

  1,  0,  0,  0,  1,  1,  0,  0,

  0,  0,  0,  0,  1,  0,  0,  0,

  0,  0,  0,  0,  0,  0,  0,  0,

  0,  0,  0,  0,  0,  0,  0,  0,

  0,  0,  0,  0,  0,  0,  0,  0,

  0,  0,  0,  0,  0,  0,  0,  0
};

static void digitalWrite(uint8_t pin, uint8_t status)
{
  if(status == LOW){
    nrf_gpio_pin_clear(pin);
  }else{
    nrf_gpio_pin_set(pin);
  }
}

static void ht1632c_select(uint8_t mask)
{
  for(uint8_t i = 0, t = 1; i < _numActivePins; ++i, t <<= 1){
    digitalWrite(_pinCS[i], (t & mask) ? LOW : HIGH);
  }
}

static void ht1632c_clear()
{
  for(uint8_t c = 0; c < NUM_CHANNEL; ++c){
    for(uint8_t i = 0; i < ADDR_SPACE_SIZE; ++i){
      mem[c][i] = 0x00; 
    }
  }
}

static void ht1632c_writeSingleBit()
{
    // Set the DATA pin to the correct state
    digitalWrite(_pinDATA, LOW);
    nrf_delay_31_25ns(2); // Delay
    // Raise the WR momentarily to allow the device to capture the data
    digitalWrite(_pinWR, HIGH);
    nrf_delay_31_25ns(2); // Delay
    // Lower it again, in preparation for the next cycle.
    digitalWrite(_pinWR, LOW);
}

static void ht1632c_writeData(uint8_t data, uint8_t len)
{
  for(int j = len - 1, t = 1 << (len - 1); j >= 0; --j, t >>= 1){
    digitalWrite(_pinDATA, ((data & t) == 0)?LOW:HIGH);
    nrf_delay_31_25ns(2);
    digitalWrite(_pinWR, HIGH);
    nrf_delay_31_25ns(2);
    digitalWrite(_pinWR, LOW);
  }
}

static void ht1632c_writeCommand(char data)
{
  ht1632c_writeData(data, HT1632_CMD_LEN);
  ht1632c_writeSingleBit();
}

static void ht1632c_renderTarget(uint8_t target)
{
  if(target < _numActivePins){
    _tgtRender = target;
  }
}

static void ht1632c_render()
{
  if(_tgtRender >= _numActivePins){
    return;
  }
  ht1632c_select(0b0001 << _tgtRender);
  ht1632c_writeData(HT1632_ID_WR, HT1632_ID_LEN);
  ht1632c_writeData(0, HT1632_ADDR_LEN);
  for(uint8_t c = 0; c < NUM_CHANNEL; ++c){
    for(uint8_t i = 0; i < ADDR_SPACE_SIZE; ++i){
      ht1632c_writeData(mem[c][i] >> HT1632_WORD_LEN, HT1632_WORD_LEN);
      ht1632c_writeData(mem[c][i], HT1632_WORD_LEN); 
    }
  }
  ht1632c_select(0);
}

static void ht1632c_initialize(uint8_t pinWR, uint8_t pinDATA)
{
  _pinWR = pinWR;
  _pinDATA = pinDATA;
  for(uint8_t i = 0; i < _numActivePins; ++i){
    nrf_gpio_cfg_output(_pinCS[i]);
  }
  nrf_gpio_cfg_output(_pinWR);
  nrf_gpio_cfg_output(_pinDATA);
  ht1632c_select(0);
  for(uint8_t i = 0; i < NUM_CHANNEL; ++i){
    mem[i] = (uint8_t *)malloc(ADDR_SPACE_SIZE);
  }
  ht1632c_clear();
  ht1632c_select(0b1111);
  ht1632c_writeData(HT1632_ID_CMD, HT1632_ID_LEN);
  ht1632c_writeCommand(HT1632_CMD_SYSDIS);
  ht1632c_writeCommand(HT1632_CMD_COMS00);
  ht1632c_writeCommand(HT1632_CMD_SYSEN); //Turn on system
  ht1632c_writeCommand(HT1632_CMD_LEDON); // Turn on LED duty cycle generator
  ht1632c_writeCommand(HT1632_CMD_PWM(16)); // PWM 16/16 duty	
  ht1632c_select(0);
	// Clear all screens by default:
  for(uint8_t i = 0; i < _numActivePins; ++i) {
    ht1632c_renderTarget(i);
    ht1632c_render();
  }
  // Set renderTarget to the first board.
  ht1632c_renderTarget(0);
}


static void show_data(uint8_t const *data,uint8_t len)
{
  uint8_t memdata = 0;
  for(uint8_t j = 0; j < len; j++){
    for(uint8_t i =0; i < 8; i++){
      memdata += (data[i+j*8] << (7-i));
    }
    mem[0][j] = memdata;
    memdata = 0;
  }
  mem[0][0] = 0xFF;
  mem[0][1] = 0x02;
}

void ht1632c_begin(uint8_t pinCS1, uint8_t pinWR, uint8_t pinDATA) 
{
  _numActivePins = 1;
  _pinCS[0] = pinCS1;
  ht1632c_initialize(pinWR, pinDATA);
}

void ht1632_Init(void)
{
  ht1632c_begin(Display_CS_Pin, Display_WR_Pin, Display_DATA_Pin);
  show_data(ht1632c_error_data,16);
  ht1632c_render();
}

void ht1632_show_image(uint8_t const *data){
  uint8_t memdata = 0;
  for(uint8_t j = 0; j < 16; j++){
    for(uint8_t i =0; i < 8; i++){
      memdata += (data[i+j*8] << (7-i));
    }
    mem[0][j] = memdata;
    memdata = 0;
  }
  ht1632c_render();
}

