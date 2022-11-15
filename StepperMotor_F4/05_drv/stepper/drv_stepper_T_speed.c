/********************************************************

				Ƕ��ʽ����ƽ̨������������㷨����

  -------------------------------------------------------
   �� �� ��   : bsp_stepper_T_speed.c
   �� �� ��   : V1.0.0
   ��    ��   : zp
   ��������   : 2022��10��30��	V1.0.0	zp
   �޶�����   : 2022��11��02��	V1.0.1	zp	����淶��
   ��������   : ��������㷨ʵ�ֺ���
   ������     : hal��
   ע            
 ********************************************************/
 
#include "drv_stepper_T_speed.h"
#include "drv_stepper_parameter.h"

#if(SPEED_CURVE == 1)

/* ϵͳ�Ӽ��ٲ��� */
SPEED_RAMP_TYPE g_motor_parameter;

/*
    ��¼�������λ��
    ϵͳ���������״̬  */

struct GLOBAL_FLAGS motor_status = {FALSE, FALSE,TRUE};



/*! \brief ���������ƶ����
 *  ͨ��������ٵ�����ٶȣ��Ը����Ĳ�����ʼ����
 *  ������ٶȺͼ��ٶȺ�С������������ƶ���������û�ﵽ����ٶȾ�Ҫ��ʼ����
 *
 *  \param move_step     �ƶ��Ĳ��� (����Ϊ˳ʱ�룬����Ϊ��ʱ��)
 *  \param acceleration  ���ٶ�,���ȡֵΪ10��   ʵ��ֵΪ 10 * 0.1 * rad / sec^2 = 1rad / sec^2
 *  \param deceleration  ���ٶ�,���ȡֵΪ10��   ʵ��ֵΪ 10 * 0.1 * rad / sec^2 = 1rad / sec^2
 *  \param max_speed     ����ٶ�,���ȡֵΪ10�� ʵ��ֵΪ 10 * 0.1 * rad / sec = 1rad / sec
 */
void Stepper_Move_Trapezoid( int32_t move_step, uint32_t acceleration, uint32_t deceleration, uint32_t max_speed)
{  
    /* �ﵽ����ٶ�ʱ�Ĳ��� */
    unsigned int max_speed_steps;
    /* ���뿪ʼ���ٵĲ����������û���ٵ�������ٶ�ʱ�� */
    unsigned int acceleration_limit_steps;

    /* ���ݲ����������ж� */
    if(move_step == 0)
    {
          /* �������Ϊ�㣬�˳�ѭ�� */
      return ;
    }
    else if(move_step < 0)
    {
        /* ���õ����ת����Ϊ��ʱ�� */
        g_motor_parameter.motor_rotate_dir = MOTOR_ROTATES_COUNTERCLOCKWISE;
        move_step = -move_step;
    }
    else
    {
        g_motor_parameter.motor_rotate_dir = MOTOR_ROTATES_CLOCKWISE;
    }	
    /* ��������ת���� */
    MOTOR_DIR(g_motor_parameter.motor_rotate_dir);
		    
    if(move_step == 1)
    {
        g_motor_parameter.accelerate_steps = -1;
        /* ����״̬ */ 
        g_motor_parameter.motor_run_state = DECEL;
        /* ����ʱ */ 
        g_motor_parameter.pulse_interval = 1000;
        /* ���õ������״̬ */ 
        motor_status.running = TRUE;
    }
		
    /* ������Ϊ����ƶ� */ 
    else if(move_step != 0)
    {
      /* ��������ٶȼ���, ����õ�min_delay���ڶ�ʱ���ļ�������ֵ   min_delay = (alpha / tt)/ w */
      g_motor_parameter.min_interval = (int32_t)(A_T_x10/max_speed);
      /* ͨ�������һ��(c0) �Ĳ�����ʱ���趨���ٶȣ�accelerationλΪ 0.1rad/sec^2 */ 
      /* pulse_interval = 1/tt * sqrt(2*alpha/accel) */ 
      /* pulse_interval = ( tfreq*0.676/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100 */ 
      g_motor_parameter.pulse_interval = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / acceleration))/10);

      /* ������ٲ�֮��ﵽ����ٶȵ�����  max_s_lim = speed^2 / (2*alpha*accel) */ 
      max_speed_steps = (uint32_t)(max_speed*max_speed/(A_x200*acceleration/10));
      /* ����ﵽ����ٶ�С��0.5�������ǽ���������Ϊ0����ʵ�����Ǳ����ƶ�����һ�����ܴﵽ��Ҫ���ٶ� */ 
      if(max_speed_steps == 0)
      {
        max_speed_steps = 1;
      }
      /* ������ٲ�֮�����Ǳ��뿪ʼ���� n1 = (n1+n2)decel / (accel + decel) */ 
      acceleration_limit_steps = (uint32_t)(move_step*deceleration/(acceleration+deceleration));
      /* ���Ǳ����������1�����ܲ��ܿ�ʼ���� */ 
      if(acceleration_limit_steps == 0)
      {
        acceleration_limit_steps = 1;
      }
      /* ʹ�������������ǿ��Լ������һ�ο�ʼ���ٵ�λ�� */ 
      if(acceleration_limit_steps <= max_speed_steps)
      {
        /* ���Ϊ��ֵ */
        g_motor_parameter.decelerate_steps = acceleration_limit_steps - move_step;
      }
      else{
        /* ���Ϊ��ֵ */
        g_motor_parameter.decelerate_steps = -(max_speed_steps*acceleration/deceleration);
      }
      /* ��ֻʣ��һ�����Ǳ������ */ 
      if(g_motor_parameter.decelerate_steps == 0)
      {
        g_motor_parameter.decelerate_steps = -1;
      }

      /* ���㿪ʼ����ʱ�Ĳ��� */ 
      g_motor_parameter.decelerate_position = move_step + g_motor_parameter.decelerate_steps;

      /* �������ٶȺ��������ǾͲ���Ҫ���м����˶� */ 
      if(g_motor_parameter.pulse_interval <= g_motor_parameter.min_interval)
      {
        g_motor_parameter.pulse_interval = g_motor_parameter.min_interval;
        g_motor_parameter.motor_run_state = RUN;
      }
      else
      {
        g_motor_parameter.motor_run_state = ACCEL;
      }
      /* ��λ���ٶȼ���ֵ */ 
      g_motor_parameter.accelerate_steps = 0;
      motor_status.running = TRUE;
    }
    /* ��ȡ��ǰ����ֵ */
    int tim_count=__HAL_TIM_GET_COUNTER(&TIM_TimeBaseStructure);
    /* �ڵ�ǰ����ֵ���������ö�ʱ���Ƚ�ֵ */
    __HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure,MOTOR_PUL_CHANNEL_x,tim_count+g_motor_parameter.pulse_interval/2); 
    /* ʹ�ܶ�ʱ��ͨ�� */
    TIM_CCxChannelCmd(MOTOR_PUL_TIM, MOTOR_PUL_CHANNEL_x, TIM_CCx_DISABLE);
    __HAL_TIM_ENABLE_IT(&TIM_TimeBaseStructure, MOTOR_TIM_IT_CCx);
    __HAL_TIM_MOE_ENABLE(&TIM_TimeBaseStructure);
    __HAL_TIM_ENABLE(&TIM_TimeBaseStructure);
}

/**
  * @brief  speed decision
  *	@note 	
  * @retval 
  */
void TSpeed_Decision()
{
	uint32_t tim_count=0;
	uint32_t tmp = 0;
	/* �����£��£�һ����ʱ���� */ 
	uint16_t new_step_delay=0;
	/* ���ٹ��������һ����ʱ���������ڣ� */ 
	static uint16_t last_accelerate_delay=0;
	/* ���ƶ����������� */ 
	static uint32_t move_step_count = 0;
    /* ������������ */
	static int32_t pulse_cycle_remainder = 0;
	/* ��ʱ��ʹ�÷�תģʽ����Ҫ���������жϲ����һ���������� */
	static uint8_t interrupt_flag_count=0;
  
	if(__HAL_TIM_GET_IT_SOURCE(&TIM_TimeBaseStructure, MOTOR_TIM_IT_CCx) !=RESET)
	{
		/* �����ʱ�ж� */ 
		__HAL_TIM_CLEAR_IT(&TIM_TimeBaseStructure, MOTOR_TIM_IT_CCx);

		/* ���ñȽ�ֵ */
		tim_count=__HAL_TIM_GET_COUNTER(&TIM_TimeBaseStructure);
		tmp = tim_count+g_motor_parameter.pulse_interval/2;
		__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure,MOTOR_PUL_CHANNEL_x,tmp);

		interrupt_flag_count++;
		if(interrupt_flag_count==2) /* 2�Σ�˵���Ѿ����һ���������� */ 
		{
			interrupt_flag_count=0;
			switch(g_motor_parameter.motor_run_state) 
			{
				/*�������ֹͣ״̬*/
				case STOP:
            move_step_count = 0;
            pulse_cycle_remainder = 0;
            /* �ر�ͨ�� */ 
            HAL_TIM_OC_Stop_IT(&TIM_TimeBaseStructure,MOTOR_PUL_CHANNEL_x);
            motor_status.running = FALSE;
            break;
        /*�����������״̬*/
        case ACCEL:
            TIM_CCxChannelCmd(MOTOR_PUL_TIM, MOTOR_PUL_CHANNEL_x, TIM_CCx_ENABLE);                                         
            move_step_count++;
            g_motor_parameter.accelerate_steps++;
            /* �����£��£�һ���������ڣ�ʱ������ */
            new_step_delay = g_motor_parameter.pulse_interval - (((2 *g_motor_parameter.pulse_interval) + pulse_cycle_remainder)/(4 * g_motor_parameter.accelerate_steps + 1));
            /* �����������´μ��㲹��������������� */ 
            pulse_cycle_remainder = ((2 * g_motor_parameter.pulse_interval)+pulse_cycle_remainder)%(4 * g_motor_parameter.accelerate_steps + 1);
            /* ����ǹ�Ӧ�ÿ�ʼ���� */
            if(move_step_count >= g_motor_parameter.decelerate_position) {
              g_motor_parameter.accelerate_steps = g_motor_parameter.decelerate_steps;
              g_motor_parameter.motor_run_state = DECEL;
            }
            /* ����Ƿ�ﵽ����������ٶ� */
            else if(new_step_delay <= g_motor_parameter.min_interval) {
              last_accelerate_delay = new_step_delay;
              new_step_delay = g_motor_parameter.min_interval;    
              pulse_cycle_remainder = 0;                          
              g_motor_parameter.motor_run_state = RUN;
            }
            break;
				/* �����������ٶ�����״̬ */
				case RUN:

            move_step_count++;
            new_step_delay = g_motor_parameter.min_interval;

            if(move_step_count >= g_motor_parameter.decelerate_position) 
            {
              g_motor_parameter.accelerate_steps = g_motor_parameter.decelerate_steps;
              /* �����һ�μ��ٵ���ʱ��Ϊ��ʼ���ٵ���ʱ */
              new_step_delay = last_accelerate_delay;
              g_motor_parameter.motor_run_state = DECEL;
            }
            break;
				/* �����������״̬ */
				case DECEL:
            move_step_count++;
            g_motor_parameter.accelerate_steps++;
            /* ������(��)һ����������(ʱ����) */
            new_step_delay = g_motor_parameter.pulse_interval - (((2 * g_motor_parameter.pulse_interval) + pulse_cycle_remainder)/(4 * g_motor_parameter.accelerate_steps + 1)); 
            /* �����������´μ��㲹��������������� */ 
            pulse_cycle_remainder = ((2 * g_motor_parameter.pulse_interval)+pulse_cycle_remainder)%(4 * g_motor_parameter.accelerate_steps + 1);
            /* ����Ƿ�Ϊ���һ�� */
            if(g_motor_parameter.accelerate_steps >= 0)
            {
              g_motor_parameter.motor_run_state = STOP;
            }
            break;
			}
			/* �����һ�μ��ʱ�� */
			g_motor_parameter.pulse_interval = new_step_delay;
		}
	}
}

#endif


