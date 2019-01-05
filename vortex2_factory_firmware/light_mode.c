#include "light_mode.h"
#include "mode_global.h"
#include "pca10056.h"
#include "nrf_log.h"
#include "neopixel_mode.h"
#include "motor_mode.h"

#define LIGHT_STACK_SIZE   128
#define LIGHT_PRIORITY   2

TaskHandle_t m_light_thread;

static void light_thread(void * arg)
{
  NRF_LOG_INFO("%s",__FUNCTION__);
  UNUSED_PARAMETER(arg);
  setBreatheColor(G);
  motor_init();
  uint32_t left = 0;
  uint32_t right = 0;
  for(;;){

    if((run_mode == control_mode) && (ble_status == ble_connected)){
        vTaskDelay(1000);
    }else{
        left = analogRead(A5);
        right = analogRead(A4);
        //NRF_LOG_INFO("left=%d",analogRead(A5));
        //NRF_LOG_INFO("right=%d",analogRead(A4));
        if((left >= right) && ((left - right) > 10) ){
            left_motor(motor_advance,400);
            right_motor(motor_advance,1000);
        }else if( (left <= right) && ((right - left) > 10)){
            left_motor(motor_advance,1000);
            right_motor(motor_advance,400);
        }else{
            left_motor(motor_advance,1000);
            right_motor(motor_advance,1000);
        }
        vTaskDelay(50);
    }
  }
}

void light_freertos_init(){
    UNUSED_VARIABLE(xTaskCreate(light_thread,
                              "light",
                              LIGHT_STACK_SIZE,
                              NULL,
                              LIGHT_PRIORITY,
                              &m_light_thread));
}