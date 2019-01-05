#include "stdint.h"
#include "reset_mode.h"
#include "mode_global.h"
#include "pca10056.h"
#include "spi_flash_mode.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_delay.h"
#include "vortex2_flash.h"
#include "neopixel_mode.h"


#define RESET_STACK_SIZE   256
#define RESET_PRIORITY   2

#define RESET_BUTTON  NRF_GPIO_PIN_MAP(0,7)

#define NFC1      NRF_GPIO_PIN_MAP(0,9)
#define NFC0      NRF_GPIO_PIN_MAP(0,10)

TaskHandle_t m_reset_thread;

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



static uint8_t testbuf[256];
static uint8_t testbuf1[256];

static void update_done(){
    vTaskSuspendAll();
    NRFX_CRITICAL_SECTION_ENTER();
    vortex2_updata_vram_messge(VRAM_ADDR+16,"APP0");                                                  
    NVIC_SystemReset();  
    NRFX_CRITICAL_SECTION_EXIT();
    if( !xTaskResumeAll () )
    {
        taskYIELD ();
    }
}

static void begin(){
}

static uint8_t writeBuff[4096];
static uint8_t readBuff[4096];

static void resetRGBshow(){
  init_neopixel();
  for(uint8_t j = 0; j < 4; j++){
    for(uint8_t i = 0; i < 4; i++){
      setColor(i, 255, 0x00, 0x00);
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
    for(uint8_t i = 0; i < 4; i++){
      setColor(i, 0x00, 0x00, 0x00);
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
  for(uint8_t i =0; i<4;i++){
    setColor(i, 255, 0x00, 0x00);
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
}

static void reset_thread(void * arg)
{
  NRF_LOG_INFO("%s",__FUNCTION__);
  UNUSED_PARAMETER(arg);
  W25QXX_Init();
  
  NRF_LOG_INFO("begin spi flash");
  /*
  for(uint32_t i =0 ; i < 16; i++){
      vortex2_flash_read_bytes(0x49000+(i*0x1000),writeBuff,4096);
      W25QXX_Write(writeBuff,0x0+(i*0x1000),4096);
  }*/
  for(;;){
  /*
    NRF_LOG_INFO("i'm reset");
    NRF_LOG_INFO("W25QXX_ReadID=0x%X",W25QXX_ReadID());
    NRF_LOG_INFO("begin reset");*/
    
    resetRGBshow();
    vTaskDelay(2000);
    for(uint32_t i = 0; i < 16; i++){
      W25QXX_Read(readBuff,0x0+ (i*0x1000),4096);
      vortex2_flash_page_erase(0x49000+i*0x1000);
      vortex2_flash_write_bytes((0x49000+i*0x1000), readBuff, 4096);
    }
    vTaskDelay(100);
    update_done();
    vTaskDelay(2000);
  }
}

void reset_freertos_init(){
    UNUSED_VARIABLE(xTaskCreate(reset_thread,
                              "reset",
                              RESET_STACK_SIZE,
                              NULL,
                              RESET_PRIORITY,
                              &m_reset_thread));

}