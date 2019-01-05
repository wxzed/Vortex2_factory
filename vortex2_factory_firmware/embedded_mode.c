#include "stdint.h"
#include "embedded_mode.h"
#include "mode_global.h"
#include "pca10056.h"
#include "buzzer_mode.h"
#include "neopixel_mode.h"
#include "spi_flash_mode.h"
#include "music_maria.h"
#include "music_joy.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_delay.h"
#include "vortex2_flash.h"

#define EMBEDDED_STACK_SIZE   128
#define EMBEDDED_PRIORITY   2

#define BUZZER_STACK_SIZE   128
#define BUZZER_PRIORITY   2

#define RESET_BUTTON  NRF_GPIO_PIN_MAP(0,7)

#define NFC1      NRF_GPIO_PIN_MAP(0,9)
#define NFC0      NRF_GPIO_PIN_MAP(0,10)

TaskHandle_t m_buzzer_thread;

static uint8_t RESET_BUTTON_FREQ = 0;
static void RESET_BUTTON_init(void)
{
  nrf_gpio_cfg_input(RESET_BUTTON,GPIO_PIN_CNF_PULL_Pullup);
}

static uint8_t  RESET_BUTTON_Status(void)
{
  return nrf_gpio_pin_read(RESET_BUTTON);
}
static void idle_state_handle(void)
{
    UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
    nrf_pwr_mgmt_run();
}


static void play_music(){
   for(uint32_t i = 0; i < (sizeof(liangliangData)/sizeof(liangliangData[0])); i++){
      int noteDuration = liangliangDurations[i] * 600;
      tone(liangliangData[i],noteDuration);
   }
   noTone();
}


static uint8_t testbuf[256];
static uint8_t testbuf1[256];
static void buzzer_thread(void * arg)
{
  NRF_LOG_INFO("%s",__FUNCTION__);
  UNUSED_PARAMETER(arg);
  RESET_BUTTON_init();
  for(;;){
    if(RESET_BUTTON_Status() == 0){
        RESET_BUTTON_FREQ++;
    }else{
        RESET_BUTTON_FREQ = 0;
    }
    if(RESET_BUTTON_FREQ > 10){
      NRF_LOG_INFO("begin reset");
      vTaskDelay(100);
      vTaskSuspendAll();
      NRFX_CRITICAL_SECTION_ENTER();
      vortex2_updata_vram_messge(VRAM_ADDR+16,"reset");                                               
      NVIC_SystemReset();  
      NRFX_CRITICAL_SECTION_EXIT();
      if( !xTaskResumeAll () )
      {
         taskYIELD ();
      }
      RESET_BUTTON_FREQ = 0;
    }
    vTaskDelay(500);
  }
}

void embedded_freertos_init(){
    UNUSED_VARIABLE(xTaskCreate(buzzer_thread,
                              "buzzer",
                              BUZZER_STACK_SIZE,
                              NULL,
                              BUZZER_PRIORITY,
                              &m_buzzer_thread));

}