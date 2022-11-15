#ifndef __BSP_STEPPER_S_SPEED_H
#define	__BSP_STEPPER_S_SPEED_H

#include "stm32f4xx.h"
#include "bsp_stepper_init.h"
#include <math.h>
#include <string.h>
#include <stdbool.h>

void Speed_Decision(void);
bool Stepper_Move_SType(int16_t motor_start_speed, int16_t motor_end_speed, float acceleration_time, int32_t motor_move_step);


#endif

