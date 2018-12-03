#include "color_mode.h"
#include "mode_global.h"
#include "pca10056.h"

#define COLOR_STACK_SIZE   256
#define COLOR_PRIORITY   2

TaskHandle_t m_color_thread;

static void blink()
{
    nrf_gpio_pin_clear(LED_4);
    vTaskDelay(400);
    nrf_gpio_pin_set(LED_4);
    vTaskDelay(400);
}

static void color_thread(void * arg)
{
  ret_code_t ret;
  nrf_gpio_cfg_output(LED_4);
  for(;;){
    blink();
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