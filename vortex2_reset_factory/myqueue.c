#include "myqueue.h"

struct BleMsg *BleBufHead = NULL;
struct BleMsg *BleBufTail = NULL;

void cuappEnqueue(uint8_t *pbuf, uint16_t len){
    struct BleMsg *p;
    p = (struct BleMsg *)malloc(sizeof(struct BleMsg) + len);
    if( p == NULL){
      free(p);
      return;
    }
    p->next = NULL;
    if(BleBufHead == NULL){
      BleBufHead = p;
      BleBufTail = p;
    }else{
      BleBufTail->next = p;
      BleBufTail = p;
    }
    p->len = len;
    p->handle = 0;
    memcpy(p->data, pbuf, len);
}

struct BleMsg *cuappDequeue(void){
    struct BleMsg *p;
    p = BleBufHead;
    if(p != NULL){
      BleBufHead = p->next;
    }
    return p;
}

