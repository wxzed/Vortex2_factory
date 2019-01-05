#ifndef MYQUEUE_MODE_H__
#define MYQUEUE_MODE_H__
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
struct BleMsg{
    struct BleMsg *next;
    uint16_t handle;
    uint16_t len;
    uint8_t data[0];
};

extern struct BleMsg *cuappDequeue(void);
extern void cuappEnqueue(uint8_t *pbuf, uint16_t len);

#endif 
