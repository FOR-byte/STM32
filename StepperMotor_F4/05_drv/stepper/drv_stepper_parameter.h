/********************************************************

				嵌入式开发平台——电机梯形算法函数

  -------------------------------------------------------
   文 件 名   : bsp_stepper_parameter.h
   版 本 号   : V1.0.0
   作    者   : zp
   生成日期   : 2022年11月03日	V1.0.0	zp
   修订日期   ：2022年11月03日	V1.0.1	zp	代码规范化
   功能描述   : 电机相关宏定义参数
   依赖于     : 
   注            
 ********************************************************/

#ifndef __BSP_STEPPER_PARAMETER_H
#define	__BSP_STEPPER_PARAMETER_H

#include "stm32f4xx.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define SPEED_CURVE 2

/******************************************** T型算法电机相关参数 ***************************************************************/
#if(SPEED_CURVE == 1)

#define FALSE             0
#define TRUE              1
#define MOTOR_ROTATES_CLOCKWISE 	      0	   /* 顺时针 */
#define MOTOR_ROTATES_COUNTERCLOCKWISE  1    /* 逆时针 */

/*电机速度决策中的四个状态*/
#define STOP              0       /* 停止状态 */ 
#define ACCEL             1       /* 加速状态 */ 
#define DECEL             2       /* 减速状态 */ 
#define RUN               3       /* 匀速状态 */ 

#define ACCEL_R(x)	ceil(x)       /* 向上取整 */
#define DECEL_R(x)	floor(x)      /* 向下取整 */

/*数学常数,用于简化计算*/
#define ALPHA             ((float)(2*3.14159/NUMBER_OF_PULSES))       /* α= 2*pi/spr */ 
#define A_T_x10           ((float)(10*ALPHA*T1_FREQ))                 /* 步距角与定时器的频率放大10倍 */
#define T1_FREQ_148       ((float)((T1_FREQ*0.676)/10))               /* 0.69为误差修正值(计算过程，文档中有写) */ 
#define A_SQ              ((float)(2*100000*ALPHA)) 
#define A_x200            ((float)(200*ALPHA))
  
/*频率相关参数*/

/*定时器实际时钟频率为：168MHz/(TIM_PRESCALER+1)
*其中 高级定时器的 频率为168MHz,其他定时器为84MHz
*168/(5+1)=28Mhz
*具体需要的频率可以自己计算
*/

#define TIM_PRESCALER      5                                          /* 梯型算法设置为 5，S型算法设置为 2-1 */
#define T1_FREQ           (SystemCoreClock/(TIM_PRESCALER+1))         /* 频率ft值 */ 

/*电机单圈参数*/
#define STEP_ANGLE				1.8					                                /* 步进电机的步距角 单位：度 */
#define FSPR              (360.0/1.8)                                 /* 步进电机的一圈所需脉冲数 */
#define MICRO_STEP                      16          			            /* 细分器细分数 */ 
#define NUMBER_OF_PULSES               (FSPR*MICRO_STEP)              /* 细分后一圈所需脉冲数 */

/*梯形加减速相关变量*/
typedef struct 
{	
	uint8_t  motor_run_state ;        /* 当前电机状态 */	
	uint8_t  motor_rotate_dir ;       /* 旋转方向 */	
	int  pulse_interval;              /* 脉冲间隔 */	
	int  decelerate_position;         /* 减速位置 */	
	int  decelerate_steps;            /* 减速步数 */	
	int  min_interval;                /* 最小间隔 */	
	int  accelerate_steps;            /* 加速步数 */
}SPEED_RAMP_TYPE;

/* 系统状态 */
struct GLOBAL_FLAGS
{
	/* 当步进电机正在运行时，值为1 */
	unsigned char running:1;
	/* 当串口接收到数据时，值为1 */
	unsigned char cmd:1;
	/* 当驱动器正常输出时,值为1 */
	unsigned char out_ena:1;
};

extern struct GLOBAL_FLAGS motor_status;

/******************************************** S型算法电机相关参数 ***************************************************************/
#elif(SPEED_CURVE == 2)

#define FORM_LEN 	   31000

typedef struct {
	__IO uint8_t  motor_status;       /* 状态 */
	__IO uint8_t  motor_direction;    /* 方向 */
	__IO uint32_t motor_position;     /* 位置 */
  __IO uint16_t pluse_time;         /* 脉冲时间 */
}STEPPER_TYPEDEF;

/* S加减速所用到的参数 */
typedef struct {
  __IO int32_t motor_start_speed;                /* 初始速度 */
  __IO int32_t motor_end_speed;                  /* 末速度 */
  __IO int32_t acclerate_total_step;             /* 加速总步数 */
  __IO int32_t start_decelerate_step;            /* 开始减速的步数 */
  __IO int32_t sum_total_step;                   /* 完整曲线总步数 */
	__IO int32_t inc_acclerate_total_step;         /* 加加速度步数 */
	__IO int32_t dec_acclerate_total_step;         /* 减加速度步数 */
  __IO float   motor_speed_form[FORM_LEN];       /* S加减速 速度表 */
}SPEEDCALC_TYPEDEF;

/* 电机速度决策中的四个状态 */
typedef enum {
  STOP = 0U,                        /* 停止状态 */
  ACCEL,                            /* 加速状态 */
  UNIFORM,                          /* 匀速状态 */
  DECEL,                            /* 减速状态 */
}StateEnum_TypeDef;

#define TIM_PRESCALER          (2 - 1)

/*频率相关参数*/
#define T1_FREQ               (SystemCoreClock / TIM_PRESCALER) /* 频率ft值 */
/*电机单圈参数*/
#define STEP_ANGLE						1.8f									            /* 步进电机的步距角 单位：度 */
#define FSPR              		(360.0f / 1.8f)                   /* 步进电机的一圈所需脉冲数 */			
#define MICRO_STEP        		16          				              /* 细分器细分数 */ 
#define SPR               		(FSPR * MICRO_STEP)               /* 细分后一圈所需脉冲数 */
#define CONVER(speed)         (float)(speed * SPR / 60.0f)      /* 根据电机转速（r/min），计算电机步速（step/s） */  
#define MIN_SPEED             (float)(T1_FREQ / 65535U)         /* 最低频率/速度 */

extern STEPPER_TYPEDEF stepper_type;

#endif





#endif


