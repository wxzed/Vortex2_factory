#include "emoticon_mode.h"
#include "mode_global.h"
#include "pca10056.h"

#define EMOTICON_STACK_SIZE   256
#define EMOTICON_PRIORITY   2

TaskHandle_t m_emoticon_thread;

static void blink()
{
    nrf_gpio_pin_clear(LED_1);
    vTaskDelay(100);
    nrf_gpio_pin_set(LED_1);
    vTaskDelay(100);
}

static void emoticon_thread(void * arg)
{
  UNUSED_PARAMETER(arg);
  ret_code_t ret;
  nrf_gpio_cfg_output(LED_1);
  while(1){
    blink();
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
