#include "rgb_mode_timer.h"
#include "neopixel_mode.h"
#include "pca10056.h"
#include "mode_global.h"
#include "nrf_log.h"

#define TIMER_PERIOD      20

#define   UP      0
#define   DOWN    1
//APP_TIMER_DEF(m_rgb_mode);

volatile static uint8_t brightness = 0;
volatile static uint8_t breatheStatus = 0;
static void setColorData(uint8_t data,uint8_t color){
  if(color == R){
    setColor( 0,  data, 0,  0);
    setColor( 1,  data, 0,  0);
    setColor( 2,  data, 0,  0);
    setColor( 3,  data, 0,  0);
  }else if(color == G){
    setColor( 0,  0,  data, 0);
    setColor( 1,  0,  data, 0);
    setColor( 2,  0,  data, 0);
    setColor( 3,  0,  data, 0);
  }else if(color == B){
    setColor( 0, 0, 0,  data);
    setColor( 1, 0, 0,  data);
    setColor( 2, 0, 0,  data);
    setColor( 3, 0, 0,  data);
  }else{  
    setColor( 0, data, data, data);
    setColor( 1, data, data, data);
    setColor( 2, data, data, data);
    setColor( 3, data, data, data);
  }
}
static void rgb_mode_timer_callback(void * pvParameter)
{
  if(beginRGBtimer){
    if(breatheStatus == UP){
      brightness += 1;
      if(brightness > 254){
        breatheStatus = DOWN;
      }
    }else{
      brightness -= 1;
      if(brightness < 1){
        breatheStatus = UP;
      }
    }
    setColorData(brightness,breatheColor);
    rgbSendStatus = true;
  }
}

void rgbMode_timer_init()
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_create(&m_rgb_mode, APP_TIMER_MODE_REPEATED, rgb_mode_timer_callback);
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_start(m_rgb_mode,APP_TIMER_TICKS(TIMER_PERIOD), NULL);
    APP_ERROR_CHECK(err_code);
    
}

void rgbMode_timer_stop(){
    app_timer_stop(m_rgb_mode);
}
void init_rgb_mode_timer()
{
    rgbMode_timer_init();
}