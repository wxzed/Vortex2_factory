
#ifndef BUZZER_MODE_H__
#define BUZZER_MODE_H__
#include "stdint.h"
#include "mode_global.h"

#define Do  262
#define Re  294
#define Mi  330
#define Fa  349
#define Sol 392
#define La  440
#define Si  494

#define buzzer_pin  26

extern void buzzer_init(void);
extern void tone(uint32_t data,uint16_t time);
extern void noTone();
#endif

