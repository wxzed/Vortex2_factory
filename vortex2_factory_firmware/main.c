/**
 * Copyright (c) 2017 - 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup usbd_ble_uart_freertos_example main.c
 * @{
 * @brief    USBD CDC ACM over BLE using FreeRTOS application main file.
 *
 * This file contains the source code for a sample application that uses the Nordic UART service
 * and USBD CDC ACM library and runs using FreeRTOS.
 * This application uses the @ref srvlib_conn_params module.
 */

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"



#include "app_timer.h"

#include "app_uart.h"
#include "app_util_platform.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_drv_usbd.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_drv_power.h"

#include "app_error.h"
#include "app_util.h"
//#include "app_usbd_serial_num.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


#include "ble_mode.h"
#include "check_mode_timer.h"
#include "color_mode.h"
#include "embedded_mode.h"
#include "emoticon_mode.h"
#include "nfc_mode.h"
#include "none_mode.h"
#include "ultrasonic_mode.h"
#include "usb_cdc_mode.h"

#define LOGGER_STACK_SIZE 512

#define LOGGER_PRIORITY 1


#define LED_BLINK_INTERVAL 800

#define BIT0  NRF_GPIO_PIN_MAP(1,10)
#define BIT1  NRF_GPIO_PIN_MAP(1,11)
#define BIT2  NRF_GPIO_PIN_MAP(1,12)
#define BIT3  NRF_GPIO_PIN_MAP(1,13)
static uint8_t modeValue = 0;

APP_TIMER_DEF(m_blink_ble);
APP_TIMER_DEF(m_blink_cdc);



#if NRF_LOG_ENABLED
static TaskHandle_t m_logger_thread;      /**< Logger thread. */
#endif

#define ENDLINE_STRING "\r\n"

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump. Can be used to identify stack location on stack unwind. */


void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

#if NRF_LOG_ENABLED

static void logger_thread(void * arg)
{
    UNUSED_PARAMETER(arg);

    while (1)
    {
        NRF_LOG_FLUSH();

        vTaskSuspend(NULL); // Suspend myself
    }
}
#endif

void vApplicationIdleHook( void )
{
#if NRF_LOG_ENABLED
     vTaskResume(m_logger_thread);
#endif
}

void log_freertos_init()
{
#if NRF_LOG_ENABLED
    if (pdPASS != xTaskCreate(logger_thread,
                              "LOGGER",
                              LOGGER_STACK_SIZE,
                              NULL,
                              LOGGER_PRIORITY,
                              &m_logger_thread))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
#endif
}


static void checkMode(){
    uint8_t BIT0Value, BIT1Value, BIT2Value, BIT3Value;
    nrf_gpio_cfg_input(BIT0, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BIT1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BIT2, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BIT3, NRF_GPIO_PIN_PULLUP);
    BIT0Value = nrf_gpio_pin_read(BIT0);
    BIT1Value = nrf_gpio_pin_read(BIT1);
    BIT2Value = nrf_gpio_pin_read(BIT2);
    BIT3Value = nrf_gpio_pin_read(BIT3);
    modeValue = ((BIT3Value << 3)) | ((BIT2Value << 2)) | ((BIT1Value << 1)) | (BIT0Value);
}

static void switchModule(){
  checkMode();
  switch(modeValue){
      case 0x0:
        emoticon_freertos_init();
        break;
      case 0x01:
        ultrasonic_freertos_init();
        break;
      case 0x02:
        color_freertos_init();
        break;
      case 0x03:
        nfc_freertos_init();
        break;
      default:
        none_freertos_init();
        break;
  }
}

int main(void)
{
    ret_code_t ret;
    log_init();
    ret = nrf_drv_power_init(NULL);
    APP_ERROR_CHECK(ret);
    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);
    log_freertos_init();

    switchModule();
    init_check_mode_timer();
    usb_cdc_freertos_init();
    ble_freertos_init();
    embedded_freertos_init();

    //SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    vTaskStartScheduler();

    for (;;)
    {
       APP_ERROR_HANDLER(NRF_ERROR_FORBIDDEN);
    }
}

/**
 * @}
 */
