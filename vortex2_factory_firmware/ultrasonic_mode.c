#include "ultrasonic_mode.h"
#include "mode_global.h"
#include "pca10056.h"

#define ULTRASONIC_STACK_SIZE   256
#define ULTRASONIC_PRIORITY   2

TaskHandle_t m_ultrasonic_thread;

static void blink()
{
    nrf_gpio_pin_clear(LED_2);
    vTaskDelay(200);
    nrf_gpio_pin_set(LED_2);
    vTaskDelay(200);
}

static void ultrasonic_thread(void * arg)
{
  UNUSED_PARAMETER(arg);
  ret_code_t ret;
  nrf_gpio_cfg_output(LED_2);
  while(1){
    blink();
  }
}

void ultrasonic_freertos_init(){

    UNUSED_VARIABLE(xTaskCreate(ultrasonic_thread,
                              "ultrasonic",
                              ULTRASONIC_STACK_SIZE,
                              NULL,
                              ULTRASONIC_PRIORITY,
                              &m_ultrasonic_thread));
}