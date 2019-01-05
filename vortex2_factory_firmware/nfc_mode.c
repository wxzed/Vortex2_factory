/*use uart*/
#include "nfc_mode.h"
#include "mode_global.h"
#include "pca10056.h"
#include "nrf_log.h"

#define NFC_STACK_SIZE   256
#define NFC_PRIORITY   2

TaskHandle_t m_nfc_thread;

static void blink()
{
    nrf_gpio_pin_clear(LED_3);
    vTaskDelay(300);
    nrf_gpio_pin_set(LED_3);
    vTaskDelay(300);
}

static void nfc_thread(void * arg)
{
  NRF_LOG_INFO("%s",__FUNCTION__);
  UNUSED_PARAMETER(arg);
  ret_code_t ret;
  nrf_gpio_cfg_output(LED_3);
  for(;;){
    blink();
  }
}

void nfc_freertos_init(){

    UNUSED_VARIABLE(xTaskCreate(nfc_thread,
                              "nfc",
                              NFC_STACK_SIZE,
                              NULL,
                              NFC_PRIORITY,
                              &m_nfc_thread));
}