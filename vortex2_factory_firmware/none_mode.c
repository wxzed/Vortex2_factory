#include "none_mode.h"
#include "mode_global.h"
#include "motor_mode.h"
#include "neopixel_mode.h"
#include "nrf_log.h"

#define NONE_STACK_SIZE   256
#define NONE_PRIORITY   2

#define right 2
#define left  3

TaskHandle_t m_none_thread;



static void none_thread(void * arg)
{
  NRF_LOG_INFO("%s",__FUNCTION__);
  UNUSED_PARAMETER(arg);
  motor_init();
  vTaskDelay(2000);
  left_motor(motor_advance,500);
  right_motor(motor_advance,500);
  //setBreatheColor(W);
  for(;;){
    if((run_mode == control_mode) && (ble_status == ble_connected)){
      vTaskDelay(1000);
    }else{
      RGB_Rotate();
    }
    //vTaskDelay(1000);
  }
}

void none_freertos_init(){

    UNUSED_VARIABLE(xTaskCreate(none_thread,
                              "none", 
                              NONE_STACK_SIZE,
                              NULL,
                              NONE_PRIORITY,
                              &m_none_thread));
}