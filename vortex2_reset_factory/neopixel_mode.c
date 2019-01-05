#include "neopixel_mode.h"
#include "pca10056.h"
#include "nrf_delay.h"


#define NEOPIXEL_STACK_SIZE   128
#define NEOPIXEL_PRIORITY   2

TaskHandle_t m_neopixel_thread;

static uint8_t data[NUM_LED *3];
volatile uint8_t breatheColor = 0;
static void setHigh(){
  nrf_gpio_pin_set(RGB_PIN);
}


static void setLow(){
  nrf_gpio_pin_clear(RGB_PIN);
}

void init_neopixel(){
  nrf_gpio_cfg_output(RGB_PIN);
  nrf_gpio_pin_clear(RGB_PIN);
  for(uint8_t i =0 ; i < NUM_LED *3; i++){
    data[i] = 0x00;
  }
}

void setBreatheColor(uint8_t value){
  breatheColor = value;
}

void setColor(uint32_t num, uint8_t r, uint8_t g, uint8_t b){
  if(num < NUM_LED){
     data[num *3] = g;
     data[num *3 + 1] = r;
     data[num *3 + 2] = b;
  }
}
static void clear()
{
  for(uint8_t i = 0; i < NUM_LED * 3; i++){
    data[i] = 0x00;
  }
  rgbSendStatus = true;
}

volatile bool rgbSendStatus = false;
void show(){
  if(rgbSendStatus){
    rgbSendStatus = false;
    nrf_delay_us(50);
    nrf_delay_31_25ns(1);
    for(uint8_t i = 0; i < NUM_LED * 3; i++){

      setHigh();
      if(data[i] & 0x80){/*HIGH*/
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(1);
      }
      setLow();
      if(data[i] & 0x80){/*HIGH*/
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(20);
      }

      setHigh();
      if(data[i] & 0x40){
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(1);
      }
      setLow();
      if(data[i] & 0x40){
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(20);
      }

      setHigh();
      if(data[i] & 0x20){
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(1);
      }
      setLow();
      if(data[i] & 0x20){
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(20);
      }


      setHigh();
      if(data[i] & 0x10){
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(1);
      }
      setLow();
      if(data[i] & 0x10){
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(20);
      }

      setHigh();
      if(data[i] & 0x08){
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(1);
      }
      setLow();
      if(data[i] & 0x08){
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(20);
      }

      setHigh();
      if(data[i] & 0x04){
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(1);
      }
      setLow();
      if(data[i] & 0x04){
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(20);
      }

      setHigh();
      if(data[i] & 0x02){
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(1);
      }
      setLow();
      if(data[i] & 0x02){
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(20);
      }

      setHigh();
      if(data[i] & 0x01){
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(1);
      }
      setLow();
      if(data[i] & 0x01){
        nrf_delay_31_25ns(11);
      }else{
        nrf_delay_31_25ns(20);
      }
    }
  }

}

void RGB_Rotate(){
    uint8_t brightness = 0x30; 
    for(uint8_t j = 0; j < 4; j++){
      for(uint8_t i = 0; i < 4; i++){
         if(j == 0){
            setColor(i, brightness, 0x00, 0x00);
            if(i > 0){
              setColor(i-1,0x00,0x00,0x00);
            }else{
              setColor(3,0x00,0x00,0x00);
            }
         }else if(j == 1){
            setColor(i, 0x00, brightness, 0x00);
            if(i > 0){
              setColor(i-1,0x00,0x00,0x00);
            }else{
              setColor(3,0x00,0x00,0x00);
            }
         }else if(j == 2){
            setColor(i, 0x00, 0x00, brightness);
            if(i > 0){
              setColor(i-1,0x00,0x00,0x00);
            }else{
              setColor(3,0x00,0x00,0x00);
            }
         }else{
            setColor(i, brightness, brightness, brightness);
            if(i > 0){
              setColor(i-1,0x00,0x00,0x00);
            }else{
              setColor(3,0x00,0x00,0x00);
            }
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
         vTaskDelay(200);
       }
    }
/*
    for(uint8_t i = 0; i < 4; i++){
      setColor(i,brightness,brightness,brightness);
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
    vTaskDelay(1000);*/
}

static void neopixel_thread(void * arg)
{
  init_neopixel();
  RGB_Rotate();
  //clear();
  for(;;){
      vTaskSuspendAll();
      NRFX_CRITICAL_SECTION_ENTER();
      show();
      NRFX_CRITICAL_SECTION_EXIT();
      if( !xTaskResumeAll () )
      {
         taskYIELD ();
      }
      vTaskDelay(50);
  }
}

void neopixel_freertos_init()
{
    UNUSED_VARIABLE(xTaskCreate(neopixel_thread,
                              "neopixel",
                              NEOPIXEL_STACK_SIZE,
                              NULL,
                              NEOPIXEL_PRIORITY,
                              &m_neopixel_thread));
}