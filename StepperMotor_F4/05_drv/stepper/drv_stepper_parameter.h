/********************************************************

				Ƕ��ʽ����ƽ̨������������㷨����

  -------------------------------------------------------
   �� �� ��   : bsp_stepper_parameter.h
   �� �� ��   : V1.0.0
   ��    ��   : zp
   ��������   : 2022��11��03��	V1.0.0	zp
   �޶�����   ��2022��11��03��	V1.0.1	zp	����淶��
   ��������   : �����غ궨�����
   ������     : 
   ע            
 ********************************************************/

#ifndef __BSP_STEPPER_PARAMETER_H
#define	__BSP_STEPPER_PARAMETER_H

#include "stm32f4xx.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define SPEED_CURVE 2

/******************************************** T���㷨�����ز��� ***************************************************************/
#if(SPEED_CURVE == 1)

#define FALSE             0
#define TRUE              1
#define MOTOR_ROTATES_CLOCKWISE 	      0	   /* ˳ʱ�� */
#define MOTOR_ROTATES_COUNTERCLOCKWISE  1    /* ��ʱ�� */

/*����ٶȾ����е��ĸ�״̬*/
#define STOP              0       /* ֹͣ״̬ */ 
#define ACCEL             1       /* ����״̬ */ 
#define DECEL             2       /* ����״̬ */ 
#define RUN               3       /* ����״̬ */ 

#define ACCEL_R(x)	ceil(x)       /* ����ȡ�� */
#define DECEL_R(x)	floor(x)      /* ����ȡ�� */

/*��ѧ����,���ڼ򻯼���*/
#define ALPHA             ((float)(2*3.14159/NUMBER_OF_PULSES))       /* ��= 2*pi/spr */ 
#define A_T_x10           ((float)(10*ALPHA*T1_FREQ))                 /* ������붨ʱ����Ƶ�ʷŴ�10�� */
#define T1_FREQ_148       ((float)((T1_FREQ*0.676)/10))               /* 0.69Ϊ�������ֵ(������̣��ĵ�����д) */ 
#define A_SQ              ((float)(2*100000*ALPHA)) 
#define A_x200            ((float)(200*ALPHA))
  
/*Ƶ����ز���*/

/*��ʱ��ʵ��ʱ��Ƶ��Ϊ��168MHz/(TIM_PRESCALER+1)
*���� �߼���ʱ���� Ƶ��Ϊ168MHz,������ʱ��Ϊ84MHz
*168/(5+1)=28Mhz
*������Ҫ��Ƶ�ʿ����Լ�����
*/

#define TIM_PRESCALER      5                                          /* �����㷨����Ϊ 5��S���㷨����Ϊ 2-1 */
#define T1_FREQ           (SystemCoreClock/(TIM_PRESCALER+1))         /* Ƶ��ftֵ */ 

/*�����Ȧ����*/
#define STEP_ANGLE				1.8					                                /* ��������Ĳ���� ��λ���� */
#define FSPR              (360.0/1.8)                                 /* ���������һȦ���������� */
#define MICRO_STEP                      16          			            /* ϸ����ϸ���� */ 
#define NUMBER_OF_PULSES               (FSPR*MICRO_STEP)              /* ϸ�ֺ�һȦ���������� */

/*���μӼ�����ر���*/
typedef struct 
{	
	uint8_t  motor_run_state ;        /* ��ǰ���״̬ */	
	uint8_t  motor_rotate_dir ;       /* ��ת���� */	
	int  pulse_interval;              /* ������ */	
	int  decelerate_position;         /* ����λ�� */	
	int  decelerate_steps;            /* ���ٲ��� */	
	int  min_interval;                /* ��С��� */	
	int  accelerate_steps;            /* ���ٲ��� */
}SPEED_RAMP_TYPE;

/* ϵͳ״̬ */
struct GLOBAL_FLAGS
{
	/* �����������������ʱ��ֵΪ1 */
	unsigned char running:1;
	/* �����ڽ��յ�����ʱ��ֵΪ1 */
	unsigned char cmd:1;
	/* ���������������ʱ,ֵΪ1 */
	unsigned char out_ena:1;
};

extern struct GLOBAL_FLAGS motor_status;

/******************************************** S���㷨�����ز��� ***************************************************************/
#elif(SPEED_CURVE == 2)

#define FORM_LEN 	   31000

typedef struct {
	__IO uint8_t  motor_status;       /* ״̬ */
	__IO uint8_t  motor_direction;    /* ���� */
	__IO uint32_t motor_position;     /* λ�� */
  __IO uint16_t pluse_time;         /* ����ʱ�� */
}STEPPER_TYPEDEF;

/* S�Ӽ������õ��Ĳ��� */
typedef struct {
  __IO int32_t motor_start_speed;                /* ��ʼ�ٶ� */
  __IO int32_t motor_end_speed;                  /* ĩ�ٶ� */
  __IO int32_t acclerate_total_step;             /* �����ܲ��� */
  __IO int32_t start_decelerate_step;            /* ��ʼ���ٵĲ��� */
  __IO int32_t sum_total_step;                   /* ���������ܲ��� */
	__IO int32_t inc_acclerate_total_step;         /* �Ӽ��ٶȲ��� */
	__IO int32_t dec_acclerate_total_step;         /* �����ٶȲ��� */
  __IO float   motor_speed_form[FORM_LEN];       /* S�Ӽ��� �ٶȱ� */
}SPEEDCALC_TYPEDEF;

/* ����ٶȾ����е��ĸ�״̬ */
typedef enum {
  STOP = 0U,                        /* ֹͣ״̬ */
  ACCEL,                            /* ����״̬ */
  UNIFORM,                          /* ����״̬ */
  DECEL,                            /* ����״̬ */
}StateEnum_TypeDef;

#define TIM_PRESCALER          (2 - 1)

/*Ƶ����ز���*/
#define T1_FREQ               (SystemCoreClock / TIM_PRESCALER) /* Ƶ��ftֵ */
/*�����Ȧ����*/
#define STEP_ANGLE						1.8f									            /* ��������Ĳ���� ��λ���� */
#define FSPR              		(360.0f / 1.8f)                   /* ���������һȦ���������� */			
#define MICRO_STEP        		16          				              /* ϸ����ϸ���� */ 
#define SPR               		(FSPR * MICRO_STEP)               /* ϸ�ֺ�һȦ���������� */
#define CONVER(speed)         (float)(speed * SPR / 60.0f)      /* ���ݵ��ת�٣�r/min�������������٣�step/s�� */  
#define MIN_SPEED             (float)(T1_FREQ / 65535U)         /* ���Ƶ��/�ٶ� */

extern STEPPER_TYPEDEF stepper_type;

#endif





#endif


