
#ifndef MODE_GLOBAL_H__
#define MODE_GLOBAL_H__
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "app_timer.h"
#include "stdbool.h"

#define   R   0
#define   G   1
#define   B   2
#define   W   3

#define   A0  2
#define   A1  3
#define   A2  4
#define   A3  5
#define   A4  28
#define   A5  29

#define ble_connected     true
#define ble_disconnect    false

#define control_mode    true
#define board_mode      false

#define emoticonCode    0x00
#define lightCode       0x01
#define colorCode       0x04
#define nfcCode         0x05
#define ultrasonicCode  0x08

extern bool ble_status;
extern bool run_mode;
extern bool beginRGBtimer;
extern uint8_t nowBoard;

APP_TIMER_DEF(m_rgb_mode);
extern TaskHandle_t m_color_thread;
extern TaskHandle_t m_embedded_thread;
extern TaskHandle_t m_neopixel_thread;
extern TaskHandle_t m_buzzer_thread;
extern TaskHandle_t m_emoticon_thread;
extern TaskHandle_t m_nfc_thread;
extern TaskHandle_t m_light_thread;
extern TaskHandle_t m_ultrasonic_thread;

extern TimerHandle_t check_mode_timer_handle;


extern volatile bool rgbSendStatus;

extern void setColor(uint32_t num, uint8_t r, uint8_t g, uint8_t b);
extern void rgbMode_timer_stop(void);
extern uint32_t analogRead(uint32_t pin);
#endif

