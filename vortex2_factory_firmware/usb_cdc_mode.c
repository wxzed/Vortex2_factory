#include "usb_cdc_mode.h"
#include "app_usbd_core.h"
#include "app_usbd.h"
#include "app_usbd_string_desc.h"
#include "app_usbd_cdc_acm.h"
#include "nrf_log.h"
#include "nrf_delay.h"

#define USB_CDC_DATA_LEN  128
#define USBD_STACK_SIZE   256
#define USBD_PRIORITY   2
#define USB_THREAD_MAX_BLOCK_TIME portMAX_DELAY

#define CDC_ACM_COMM_INTERFACE  0
#define CDC_ACM_COMM_EPIN       NRF_DRV_USBD_EPIN2

#define CDC_ACM_DATA_INTERFACE  1
#define CDC_ACM_DATA_EPIN       NRF_DRV_USBD_EPIN1
#define CDC_ACM_DATA_EPOUT      NRF_DRV_USBD_EPOUT1
static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_cdc_acm_user_event_t event);
static TaskHandle_t m_usbd_thread;      
static bool m_usb_connected = false;
static char m_cdc_data_array[USB_CDC_DATA_LEN];

APP_USBD_CDC_ACM_GLOBAL_DEF(m_app_cdc_acm,
                            cdc_acm_user_ev_handler,
                            CDC_ACM_COMM_INTERFACE,
                            CDC_ACM_DATA_INTERFACE,
                            CDC_ACM_COMM_EPIN,
                            CDC_ACM_DATA_EPIN,
                            CDC_ACM_DATA_EPOUT,
                            APP_USBD_CDC_COMM_PROTOCOL_AT_V250);

static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_cdc_acm_user_event_t event)
{
    app_usbd_cdc_acm_t const * p_cdc_acm = app_usbd_cdc_acm_class_get(p_inst);

    switch (event)
    {
        case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
        {
            ret_code_t ret = app_usbd_cdc_acm_read(&m_app_cdc_acm,
                                                   m_cdc_data_array,
                                                   1);
            UNUSED_VARIABLE(ret);
            NRF_LOG_INFO("CDC ACM port opened");
            break;
        }

        case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
            NRF_LOG_INFO("CDC ACM port closed");
            if (m_usb_connected)
            {
            }
            break;

        case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
            break;

        case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
        {
            break;
        }
        default:
            break;
    }
}

static void usbd_user_ev_handler(app_usbd_event_type_t event)
{
    switch (event)
    {
        case APP_USBD_EVT_DRV_SUSPEND:
            break;

        case APP_USBD_EVT_DRV_RESUME:
            break;

        case APP_USBD_EVT_STARTED:
            break;

        case APP_USBD_EVT_STOPPED:
            app_usbd_disable();
            break;

        case APP_USBD_EVT_POWER_DETECTED:
            NRF_LOG_INFO("USB power detected");

            if (!nrf_drv_usbd_is_enabled())
            {
                app_usbd_enable();
            }
            break;

        case APP_USBD_EVT_POWER_REMOVED:
        {
            NRF_LOG_INFO("USB power removed");
            m_usb_connected = false;
            app_usbd_stop();
        }
            break;

        case APP_USBD_EVT_POWER_READY:
        {
            NRF_LOG_INFO("USB ready");
            m_usb_connected = true;
            app_usbd_start();
        }
            break;

        default:
            break;
    }
}

void usb_new_event_isr_handler(app_usbd_internal_evt_t const * const p_event, bool queued)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    UNUSED_PARAMETER(p_event);
    UNUSED_PARAMETER(queued);
    ASSERT(m_usbd_thread != NULL);
    /* Release the semaphore */
    vTaskNotifyGiveFromISR(m_usbd_thread, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void usbd_thread(void * arg)
{
    ret_code_t ret;
    static const app_usbd_config_t usbd_config = {
        .ev_isr_handler = usb_new_event_isr_handler,
        .ev_state_proc  = usbd_user_ev_handler
    };
    UNUSED_PARAMETER(arg);

    ret = app_usbd_init(&usbd_config);
    APP_ERROR_CHECK(ret);
    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&m_app_cdc_acm);
    ret = app_usbd_class_append(class_cdc_acm);
    APP_ERROR_CHECK(ret);
    ret = app_usbd_power_events_enable();
    APP_ERROR_CHECK(ret);
    UNUSED_RETURN_VALUE(xTaskNotifyGive(xTaskGetCurrentTaskHandle()));
    for (;;)
    {
        UNUSED_RETURN_VALUE(ulTaskNotifyTake(pdTRUE, USB_THREAD_MAX_BLOCK_TIME));
        while (app_usbd_event_queue_process())
        {
        }

    }
}

void usb_cdc_freertos_init()
{
    if (pdPASS != xTaskCreate(usbd_thread,
                              "USBD",
                              USBD_STACK_SIZE,
                              NULL,
                              USBD_PRIORITY,
                              &m_usbd_thread))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
}
