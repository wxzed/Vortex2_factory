#include "embedded_mode.h"
#include "mode_global.h"
#include "pca10056.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"

#define EMBEDDED_STACK_SIZE   256
#define EMBEDDED_PRIORITY   2

#define NFC0      NRF_GPIO_PIN_MAP(0,9)
#define NFC1      NRF_GPIO_PIN_MAP(0,10)
#define BUZZER    NRF_GPIO_PIN_MAP(1,01)

TaskHandle_t m_embedded_thread;


static void idle_state_handle(void)
{
    UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
    nrf_pwr_mgmt_run();
}

static void embedded_thread(void * arg)
{
  ret_code_t ret;
  UNUSED_PARAMETER(arg);
  for(;;){
    vTaskDelay(1000);
    //idle_state_handle();
  }
}

void embedded_freertos_init(){

    UNUSED_VARIABLE(xTaskCreate(embedded_thread,
                              "embedded",
                              EMBEDDED_STACK_SIZE,
                              NULL,
                              EMBEDDED_PRIORITY,
                              &m_embedded_thread));
}