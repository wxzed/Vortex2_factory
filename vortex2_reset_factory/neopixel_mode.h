
#ifndef NEOPIXEL_MODE_H__
#define NEOPIXEL_MODE_H__
#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"

#define NUM_LED   4
#define RGB_PIN    NRF_GPIO_PIN_MAP(1,13)

__STATIC_INLINE void nrfx_coredep_delay_31_25ns(uint32_t time_ns)
{
    if (time_ns == 0)
    {
        return;
    }
    __ALIGN(16)
    static const uint16_t delay_bytecode[] = {
        0x3803, // SUBS r0, #3
        0xd8fd, // BHI .-2
        0x4770  // BX LR
        };

    typedef void (* delay_func_t)(uint32_t);
    // Set LSB to 1 to execute code in Thumb mode.
    const delay_func_t delay_cycles = (delay_func_t)((((uint32_t)delay_bytecode) | 1));
    uint32_t cycles = time_ns * 2;
    delay_cycles(cycles);
}

#define nrf_delay_31_25ns(ns_time) nrfx_coredep_delay_31_25ns(ns_time)
extern volatile uint8_t breatheColor;
extern volatile bool rgbSendStatus;
extern void setColor(uint32_t num, uint8_t r, uint8_t g, uint8_t b);
extern void init_neopixel();
extern void show();
extern void neopixel_freertos_init();
extern void setBreatheColor(uint8_t value);
extern void RGB_Rotate();
#endif

