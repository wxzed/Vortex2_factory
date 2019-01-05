
#ifndef BLE_MODE_H__
#define BLE_MODE_H__
#include "FreeRTOS.h"
#include "task.h"

extern bool ble_stauts;
void ble_freertos_init(void);
void send_ble_data(uint8_t *data,uint16_t len);
#endif

