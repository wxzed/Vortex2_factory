#include "color_mode.h"
#include "mode_global.h"
#include "neopixel_mode.h"
#include "pca10056.h"
#include "nrf_log.h"
#include "app_timer.h"
#include "i2c_mode.h"

#define COLOR_STACK_SIZE   256
#define COLOR_PRIORITY   2
#define COLOR_LED   15
TaskHandle_t m_color_thread;

static bool led_status = false;
static bool color_change = false;

static bool _tcs34725Initialised = false;
static tcs34725IntegrationTime_t  _tcs34725IntegrationTime = TCS34725_INTEGRATIONTIME_50MS;
static tcs34725Gain_t  _tcs34725Gain = TCS34725_GAIN_4X;


static void init_color_mode()
{
  nrf_gpio_cfg_output(COLOR_LED);
  nrf_gpio_pin_set(COLOR_LED);
}

static void open_light()
{
  nrf_gpio_pin_set(COLOR_LED);
  led_status = true;
}

static void close_light()
{
  nrf_gpio_pin_clear(COLOR_LED);
  led_status  =false;
}


static void writeReg(uint8_t reg, uint32_t value)
{
  i2c_beginTransmission(TCS34725_ADDRESS);
  writebyte(TCS34725_COMMAND_BIT | reg);
  writebyte(value & 0xFF);
  i2c_endTransmission();
}

static uint8_t readReg(uint8_t reg)
{
  i2c_beginTransmission(TCS34725_ADDRESS);
  writebyte(TCS34725_COMMAND_BIT | reg);
  i2c_endTransmission();
  i2c_requestFrom(TCS34725_ADDRESS, 1);
  return i2c_read();
}

static uint16_t readRegWord(uint8_t reg)
{
  uint16_t x; uint16_t t;
  i2c_beginTransmission(TCS34725_ADDRESS);
  writebyte(TCS34725_COMMAND_BIT | reg);
  i2c_endTransmission();
  i2c_requestFrom(TCS34725_ADDRESS, 2);
  t = i2c_read();
  x = i2c_read();
  x <<= 8;
  x |= t;
  return x;
}



static void setIntegrationTime(tcs34725IntegrationTime_t it)
{
  if (!_tcs34725Initialised) NRF_LOG_INFO("!_tcs34725Initialised");
  writeReg(TCS34725_ATIME, it);
  _tcs34725IntegrationTime = it;
}

static void setGain(tcs34725Gain_t gain)
{
  if (!_tcs34725Initialised) NRF_LOG_INFO("!_tcs34725Initialised");

  writeReg(TCS34725_CONTROL, gain);

  _tcs34725Gain = gain;
}
static void enable(void)
{
  writeReg(TCS34725_ENABLE, TCS34725_ENABLE_PON);
  vTaskDelay(3);
  writeReg(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);  
}
static bool TCS34725_begin()
{
  twi_init();
  uint8_t x = readReg(TCS34725_ID);
  if((x != 0x44) && (x != 0x10)){
    return false;
  }
  _tcs34725Initialised = true;
  setIntegrationTime(_tcs34725IntegrationTime);
  setGain(_tcs34725Gain);
  enable();
  return true;
}

static void getRGBC(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c)
{
  *c = readRegWord(TCS34725_CDATAL);
  *r = readRegWord(TCS34725_RDATAL);
  *g = readRegWord(TCS34725_GDATAL);
  *b = readRegWord(TCS34725_BDATAL);
  switch (_tcs34725IntegrationTime)
  {
    case TCS34725_INTEGRATIONTIME_2_4MS:
      vTaskDelay(3);
      break;
    case TCS34725_INTEGRATIONTIME_24MS:
      vTaskDelay(24);
      break;
    case TCS34725_INTEGRATIONTIME_50MS:
      vTaskDelay(50);
      break;
    case TCS34725_INTEGRATIONTIME_101MS:
      vTaskDelay(101);
      break;
    case TCS34725_INTEGRATIONTIME_154MS:
      vTaskDelay(154);
      break;
    case TCS34725_INTEGRATIONTIME_700MS:
      vTaskDelay(700);
      break;
  }
}

static void lock()
{
  uint8_t r = readReg(TCS34725_ENABLE);
  r |= TCS34725_ENABLE_AIEN;
  writeReg(TCS34725_ENABLE, r);
}


static void color_thread(void * arg)
{
  static uint16_t last_red = 0;
  static uint16_t last_green = 0;
  static uint16_t last_blue = 0;
  static uint16_t last_clear = 0; 
  
  static bool first = true;

  static uint16_t real_red, real_green, real_blue;
  NRF_LOG_INFO("%s",__FUNCTION__);
  UNUSED_PARAMETER(arg);
  ret_code_t ret;
  vTaskDelay(500);
  rgbMode_timer_stop();
  uint8_t readColor = 0x30;
  init_color_mode();
  if(TCS34725_begin()){
    NRF_LOG_INFO("Found sensor");
  }else{
    NRF_LOG_INFO("No TCS34725");
  }
  //open_light();
  close_light();
  motor_init();
  for(;;){
    if((run_mode == control_mode) && (ble_status == ble_connected)){
        vTaskDelay(1000);
    }else{
        uint16_t clear, red, green, blue;
        getRGBC(&red, &green, &blue, &clear);
        lock();
        if(clear < 60 && clear > 0){
            open_light();
        }else if(clear > 5000 && clear < 9000){
            close_light();
        }
        if(led_status){
            if(((last_clear - clear) < 1000) && ((last_clear - clear) > -1000)){
              color_change = true;
              real_red = red;
              real_green = green;
              real_blue = blue;
            }
            last_clear = clear;
        }else{
            color_change = false;
        }
        if(color_change){
            float sum_r,sum_g,sum_b;
            sum_r = real_red / (float)clear;
            sum_g = real_green / (float)clear;
            sum_b = real_blue / (float)clear;
            for(uint8_t i =0; i < NUM_LED; i++){
            setColor(i, (uint8_t)(sum_r * 256),  (uint8_t)(sum_g * 256),  (uint8_t)(sum_b * 256));
        }
        rgbSendStatus = true;
        vTaskSuspendAll();
        NRFX_CRITICAL_SECTION_ENTER();
        show();
        NRFX_CRITICAL_SECTION_EXIT();
        if( !xTaskResumeAll () )
        {
          taskYIELD ();
        }
      }
      vTaskDelay(100);
    }
  }
}

void color_freertos_init(){

    UNUSED_VARIABLE(xTaskCreate(color_thread,
                              "color",
                              COLOR_STACK_SIZE,
                              NULL,
                              COLOR_PRIORITY,
                              &m_color_thread));
}