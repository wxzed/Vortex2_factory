#include "none_mode.h"
#include "mode_global.h"

#define NONE_STACK_SIZE   256
#define NONE_PRIORITY   2

TaskHandle_t m_none_thread;

static void none_thread(void * arg)
{
  UNUSED_PARAMETER(arg);
  for(;;){
    vTaskDelay(1000);
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