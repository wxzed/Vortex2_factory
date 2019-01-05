
#ifndef MOTOR_MODE_H__
#define MOTOR_MODE_H__
#include "stdint.h"


#define motor_advance   0
#define motor_retreat   1
#define motor_stop      2



extern void motor_init(void);
extern void right_motor(uint8_t direction ,uint32_t speed);
extern void left_motor(uint8_t direction ,uint32_t speed);
extern uint32_t M1A_DATA;
extern uint32_t M1B_DATA;
extern uint32_t M2A_DATA;
extern uint32_t M2B_DATA;
#endif

