#include "emoticon_mode.h"
#include "mode_global.h"
#include "neopixel_mode.h"
#include "pca10056.h"
#include "nrf_log.h"
#include "motor_mode.h"
#include "ht1632c.h"
#include "stdlib.h"
/*for line*/
#define EMOTICON_STACK_SIZE   256
#define EMOTICON_PRIORITY   2
#define Left_pin    4
#define Right_pin   5
#define left    0
#define right   1
#define TurnLeft  1
#define TurnRight 2
#define Advance   0

TaskHandle_t m_emoticon_thread;

static uint32_t saadcReference = SAADC_CH_CONFIG_REFSEL_VDD1_4;
static int readResolution = 10;

static inline uint32_t mapResolution( uint32_t value, uint32_t from, uint32_t to )
{
  if ( from == to )
  {
    return value ;
  }

  if ( from > to )
  {
    return value >> (from-to) ;
  }
  else
  {
    return value << (to-from) ;
  }
}

uint32_t analogRead(uint32_t pin)
{
  uint32_t saadcResolution;
  uint32_t resolution;
  int16_t value;
  switch (pin) {
    case 2:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput0;
      break;

    case 3:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput1;
      break;

    case 4:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput2;
      break;

    case 5:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput3;
      break;

    case 28:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput4;
      break;

    case 29:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput5;
      break;

    case 30:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput6;
      break;

    case 31:
      pin = SAADC_CH_PSELP_PSELP_AnalogInput7;
      break;

    default:
      return 0;
  }

  if (readResolution <= 8) {
    resolution = 8;
    saadcResolution = SAADC_RESOLUTION_VAL_8bit;
  } else if (readResolution <= 10) {
    resolution = 10;
    saadcResolution = SAADC_RESOLUTION_VAL_10bit;
  } else if (readResolution <= 12) {
    resolution = 12;
    saadcResolution = SAADC_RESOLUTION_VAL_12bit;
  } else {
    resolution = 14;
    saadcResolution = SAADC_RESOLUTION_VAL_14bit;
  }

  NRF_SAADC->RESOLUTION = saadcResolution;

  NRF_SAADC->ENABLE = (SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos);
  for (int i = 0; i < 8; i++) {
    NRF_SAADC->CH[i].PSELN = SAADC_CH_PSELP_PSELP_NC;
    NRF_SAADC->CH[i].PSELP = SAADC_CH_PSELP_PSELP_NC;
  }
  NRF_SAADC->CH[0].CONFIG = ((SAADC_CH_CONFIG_RESP_Bypass   << SAADC_CH_CONFIG_RESP_Pos)   & SAADC_CH_CONFIG_RESP_Msk)
                            | ((SAADC_CH_CONFIG_RESP_Bypass   << SAADC_CH_CONFIG_RESN_Pos)   & SAADC_CH_CONFIG_RESN_Msk)
                            | ((SAADC_CH_CONFIG_GAIN_Gain1_4    << SAADC_CH_CONFIG_GAIN_Pos)   & SAADC_CH_CONFIG_GAIN_Msk)
                            | ((saadcReference                << SAADC_CH_CONFIG_REFSEL_Pos) & SAADC_CH_CONFIG_REFSEL_Msk)
                            | ((SAADC_CH_CONFIG_TACQ_3us      << SAADC_CH_CONFIG_TACQ_Pos)   & SAADC_CH_CONFIG_TACQ_Msk)
                            | ((SAADC_CH_CONFIG_MODE_SE       << SAADC_CH_CONFIG_MODE_Pos)   & SAADC_CH_CONFIG_MODE_Msk);
  NRF_SAADC->CH[0].PSELN = pin;
  NRF_SAADC->CH[0].PSELP = pin;


  NRF_SAADC->RESULT.PTR = (uint32_t)&value;
  NRF_SAADC->RESULT.MAXCNT = 1; // One sample

  NRF_SAADC->TASKS_START = 0x01UL;

  while (!NRF_SAADC->EVENTS_STARTED);
  NRF_SAADC->EVENTS_STARTED = 0x00UL;

  NRF_SAADC->TASKS_SAMPLE = 0x01UL;

  while (!NRF_SAADC->EVENTS_END);
  NRF_SAADC->EVENTS_END = 0x00UL;

  NRF_SAADC->TASKS_STOP = 0x01UL;

  while (!NRF_SAADC->EVENTS_STOPPED);
  NRF_SAADC->EVENTS_STOPPED = 0x00UL;

  if (value < 0) {
    value = 0;
  }
  NRF_SAADC->ENABLE = (SAADC_ENABLE_ENABLE_Disabled << SAADC_ENABLE_ENABLE_Pos);
  return mapResolution(value, resolution, readResolution);
}

static void emoticon_thread(void * arg)
{
  NRF_LOG_INFO("%s",__FUNCTION__);
  UNUSED_PARAMETER(arg);
  NRF_LOG_INFO("adc0 = %d",analogRead(4));
  setBreatheColor(B);
  motor_init();
  uint16_t leftValue = 0;
  uint16_t rightValue = 0;
  uint16_t bigValue = left;
  uint8_t car_status = 3;
  ht1632_Init();
  while(1){
    leftValue = analogRead(Left_pin);
    rightValue = analogRead(Right_pin);
    NRF_LOG_INFO("leftValue=%d\r\n",leftValue);
    NRF_LOG_INFO("rightValue =%d\r\n",rightValue);
    if(leftValue > 200 || rightValue > 200){
      if(car_status != Advance){
        car_status = Advance;
        ht1632_show_image(ht1632c_Advance_data);
      }
      left_motor(motor_advance,1000);
      right_motor(motor_advance,1000);
      bigValue = ((leftValue > rightValue) ? left : right);
    }else if(leftValue < 100 && rightValue < 100){
      if(bigValue  == left){/**/
        if(car_status != TurnLeft){
          ht1632_show_image(ht1632c_TurnLeft_data);
          car_status = TurnLeft;
        }
        left_motor(motor_advance,200);
        right_motor(motor_advance,800);
      }else{
        if(car_status != TurnRight){
          ht1632_show_image(ht1632c_TurnRight_data);
        }
        left_motor(motor_advance,800);
        right_motor(motor_advance,200);
      }
    }else{
      if((rand()%2) == 0){
          left_motor(motor_advance,1000);
          right_motor(motor_retreat,1000);
      }else{
          left_motor(motor_retreat,1000);
          right_motor(motor_advance,1000);
      }
    }
    vTaskDelay(50);
  }
}
void emoticon_freertos_init(){
  UNUSED_VARIABLE(xTaskCreate(emoticon_thread,
                              "emoticon",
                              EMOTICON_STACK_SIZE,
                              NULL,
                              EMOTICON_PRIORITY,
                              &m_emoticon_thread));
}
