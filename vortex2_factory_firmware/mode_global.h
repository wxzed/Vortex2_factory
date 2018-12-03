
#ifndef MODE_GLOBAL_H__
#define MODE_GLOBAL_H__
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"


extern TaskHandle_t m_color_thread;
extern TaskHandle_t m_embedded_thread;
extern TaskHandle_t m_emoticon_thread;
extern TaskHandle_t m_nfc_thread;
extern TaskHandle_t m_ultrasonic_thread;

extern TimerHandle_t check_mode_timer_handle;

#endif

