#ifndef __BSP_STEPPER_T_SPEED_H
#define	__BSP_STEPPER_T_SPEED_H

#include "stm32f4xx.h"
#include "bsp_stepper_init.h"


void TSpeed_Decision(void);
void Stepper_Move_Trapezoid( int32_t move_step, uint32_t acceleration, uint32_t deceleration, uint32_t max_speed);

#endif
