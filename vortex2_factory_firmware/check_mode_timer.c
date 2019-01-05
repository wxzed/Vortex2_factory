#include "check_mode_timer.h"
#include "pca10056.h"
#include "app_timer.h"
#include "mode_global.h"
#include "nrf_log.h"

#define BIT0  NRF_GPIO_PIN_MAP(1,06)
#define BIT1  NRF_GPIO_PIN_MAP(1,04)
#define BIT2  NRF_GPIO_PIN_MAP(1,02)
#define BIT3  NRF_GPIO_PIN_MAP(1,00)


#define TIMER_PERIOD      1000
#define testPin LED_2

APP_TIMER_DEF(m_check_mode);


static uint8_t BIT0Value,BIT1Value,BIT2Value,BIT3Value;
static uint8_t modeValue = 0;
static uint8_t tempValue = 0;
static void check_mode_timer_callback(void * pvParameter)
{
    BIT0Value = nrf_gpio_pin_read(BIT0);
    BIT1Value = nrf_gpio_pin_read(BIT1);
    BIT2Value = nrf_gpio_pin_read(BIT2);
    BIT3Value = nrf_gpio_pin_read(BIT3);
    modeValue = ((BIT3Value << 3)) | ((BIT2Value << 2)) | ((BIT1Value << 1)) | (BIT0Value);
    if(modeValue != tempValue){
      __disable_irq();                                                   
      NVIC_SystemReset();                                                
    }
    
}

static void checkMode_timer_init()
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_create(&m_check_mode, APP_TIMER_MODE_REPEATED, check_mode_timer_callback);
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_start(m_check_mode,APP_TIMER_TICKS(1000), (void *) LED_2);
    APP_ERROR_CHECK(err_code);
}
void init_check_mode_timer()
{


    nrf_gpio_cfg_input(BIT0, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BIT1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BIT2, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BIT3, NRF_GPIO_PIN_PULLUP);
    BIT0Value = nrf_gpio_pin_read(BIT0);
    BIT1Value = nrf_gpio_pin_read(BIT1);
    BIT2Value = nrf_gpio_pin_read(BIT2);
    BIT3Value = nrf_gpio_pin_read(BIT3);
    modeValue = ((BIT3Value << 3)) | ((BIT2Value << 2)) | ((BIT1Value << 1)) | (BIT0Value);
    tempValue = modeValue;
    checkMode_timer_init();
}