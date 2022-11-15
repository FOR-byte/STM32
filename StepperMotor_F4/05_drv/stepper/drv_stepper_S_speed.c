/********************************************************

				Ƕ��ʽ����ƽ̨�������S���㷨����

  -------------------------------------------------------
   �� �� ��   : bsp_stepper_S_speed.c
   �� �� ��   : V1.0.0
   ��    ��   : zp
   ��������   : 2022��11��03��	V1.0.0	zp
   �޶�����   : 2022��11��04��	V1.0.1	zp
   ��������   : ���S���㷨ʵ�ֺ���
   ������     : 
   ע            
 ********************************************************/
 
#include "drv_stepper_S_speed.h"
#include "drv_stepper_parameter.h"

#if(SPEED_CURVE == 2)

/* �㷨��ؽṹ��������� */
SPEEDCALC_TYPEDEF s_speed_parameter;
STEPPER_TYPEDEF stepper_type = {0};

static bool CalcSpeed(int32_t motor_start_speed, int32_t motor_end_speed, float motor_acclerate_time)
{
  int32_t pulse_period_count = 0;           /* ��������־λ */  
  float midpoint_speed =0.0f;               /* �м���ٶ� */
  float double_acceleration = 0.0f;         /* �Ӽ��ٶ� */
  float time_interval = 0.0f;               /* ʱ���� */
  float time_accumulation = 0.0f;           /* ʱ���ۼ��� */  
  float speed_increment = 0.0f;             /* �ٶȵ����� */
  float temporary_variable = 0.0f;          /* �м���� */                
  
  /* ������ٶȡ�ĩ�ٶ� */
  s_speed_parameter.motor_start_speed = CONVER(motor_start_speed);
  s_speed_parameter.motor_end_speed = CONVER(motor_end_speed);
  
  /* �����ʼ���� */
  
  /* �Ӽ��ٶε�ʱ�䣨���ٶ�б�ʴ���0��ʱ�䣩 */
  motor_acclerate_time = motor_acclerate_time / 2.0f;                 
  /* �����е���ٶ� */  
  midpoint_speed = (s_speed_parameter.motor_start_speed + s_speed_parameter.motor_end_speed) / 2.0f;    
  /* �����е��ٶȼ���Ӽ��ٶ� */
  double_acceleration = fabsf((2.0f * (midpoint_speed - s_speed_parameter.motor_start_speed)) / (motor_acclerate_time * motor_acclerate_time));  
  /* �Ӽ�����Ҫ�Ĳ��� */ 
  s_speed_parameter.inc_acclerate_total_step = (int32_t)((s_speed_parameter.motor_start_speed * motor_acclerate_time) + ((double_acceleration * motor_acclerate_time * motor_acclerate_time * motor_acclerate_time) / 6.0f));
  /* ��������Ҫ�Ĳ��� */
  s_speed_parameter.dec_acclerate_total_step = (int32_t)(((s_speed_parameter.motor_end_speed + s_speed_parameter.motor_start_speed) * motor_acclerate_time - s_speed_parameter.inc_acclerate_total_step));
  /* �����ܹ���Ҫ�Ĳ��� */
  s_speed_parameter.acclerate_total_step = s_speed_parameter.inc_acclerate_total_step + s_speed_parameter.dec_acclerate_total_step;
  if(s_speed_parameter.acclerate_total_step % 2 != 0 )
    s_speed_parameter.acclerate_total_step += 1;
  
  /* �ж��ڴ��С */
  if(FORM_LEN <s_speed_parameter.acclerate_total_step)
  {
    return false;
  }
  
  /* �����һ����ʱ�� */
	/* ���ݵ�һ����ʱ����㣬��һ�����ٶȺ�����ʱ���� */
	/* ����λ��Ϊ0��ʱ������������ʱ��Ĺ�ϵʽ -> ����λ�ƺ�ʱ��Ĺ�ʽS = 1/2 * K * Ti^3  �ɵ� Ti=6 * 1 / K��1/3�η� */
  
  /* �������ʱ�䳣�� */
  time_interval = pow(6.0f *1.0f / double_acceleration,1.0f / 3.0f);
  /* �ۼ�ʱ�䳣�� */
  time_accumulation += time_interval; 
  /* ����V=1/2*K*T^2,���Լ����һ�����ٶ� */
  speed_increment = 0.5f * double_acceleration * time_accumulation * time_accumulation;
  /* �ڳ�ʼ�ٶ������Ӽ��ٶ� */
  s_speed_parameter.motor_speed_form[0] = s_speed_parameter.motor_start_speed + speed_increment;
  /* ��С�ٶ��޷� */  
  /* �Ե�ǰ��ʱ��Ƶ�����ܵ��������ٶ� */
  if(s_speed_parameter.motor_speed_form[0] <= MIN_SPEED)
  {
    s_speed_parameter.motor_speed_form[0] = MIN_SPEED;
  }
  /* ����S���ٶȱ� */
  for(pulse_period_count = 1;pulse_period_count < s_speed_parameter.acclerate_total_step;pulse_period_count++)
  {
    /* ����ʱ��������Ƶ�ʳɷ��ȵĹ�ϵ�����Լ����Ti,������ÿ�μ�����һ��ʱ�䣬���ڻ��۵���ǰʱ�� */
    time_interval = 1.0f / s_speed_parameter.motor_speed_form[pulse_period_count-1];
    if(pulse_period_count < s_speed_parameter.inc_acclerate_total_step)
    {
			/* �ۻ�ʱ�� */
      time_accumulation += time_interval;
			/* �ٶȵı仯�� dV = 1/2 * K * Ti^2 */
      speed_increment = 0.5f * double_acceleration * time_accumulation * time_accumulation;
			/* ���ݳ�ʼ�ٶȺͱ仯������ٶȱ� */
      s_speed_parameter.motor_speed_form[pulse_period_count] = s_speed_parameter.motor_start_speed + speed_increment;
			/* Ϊ�˱�֤�����һ������ʹ��ʱ���Ͻ�����Ԥ�ڼ����ʱ��һ�£������һ�����д��� */
      if(pulse_period_count == s_speed_parameter.inc_acclerate_total_step - 1)
        {time_accumulation  = fabsf(time_accumulation - motor_acclerate_time);}
    }
    /* �����ٶȼ��� */
    else
    {
			/* ʱ���ۻ� */
      time_accumulation += time_interval;
			/* �����ٶ� */
      temporary_variable = fabsf(motor_acclerate_time - time_accumulation);
      speed_increment = 0.5f * double_acceleration * temporary_variable * temporary_variable;
      s_speed_parameter.motor_speed_form[pulse_period_count] = s_speed_parameter.motor_end_speed - speed_increment;
    }
  }
  return true;  
}  

/**
  * @brief  �ٶȾ���
  * @param  pScurve��S���߽ṹ��ָ��
  * @retval ��
	*	@note 	���ж���ʹ�ã�ÿ��һ���жϣ�����һ��
  */
void Speed_Decision(void)
{
  /* ������� */
  static __IO uint8_t i = 0;
  static __IO uint32_t index = 0;
  
  if(__HAL_TIM_GET_IT_SOURCE(&TIM_TimeBaseStructure, MOTOR_TIM_IT_CCx) != RESET)
  {
    /* �����ʱ���ж� */
		__HAL_TIM_CLEAR_IT(&TIM_TimeBaseStructure, MOTOR_TIM_IT_CCx);
    /* ��������Ϊһ������ */
    i++;
    if(i==2)
    {
      i=0;
      switch(stepper_type.motor_status)
      {
        case ACCEL:
          if(stepper_type.motor_position >= (s_speed_parameter.acclerate_total_step - 1))
          {
            stepper_type.motor_status = UNIFORM;
            index -= 1;
            break;
          }
          /* ��ȡÿһ���Ķ�ʱ������ֵ */
          stepper_type.pluse_time = (uint16_t)(T1_FREQ / s_speed_parameter.motor_speed_form[index] / 2U);
          /* ����λ���������� */
          index++;
          break;
        case DECEL:
          if(stepper_type.motor_position >= (s_speed_parameter.sum_total_step - 1))
          {
            /* ����ֹͣ״̬������ٶȱ��ҹر����ͨ�� */
            HAL_TIM_OC_Stop_IT(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x);
            memset((void*)s_speed_parameter.motor_speed_form, 0, sizeof(float) * FORM_LEN);
            index = 0;
            stepper_type.motor_status = STOP;
            break;
          }
          /* ��ȡÿһ���Ķ�ʱ������ֵ */
          stepper_type.pluse_time = (uint16_t)(T1_FREQ / s_speed_parameter.motor_speed_form[index] / 2U);
          /* ����λ���������� */
          index--;
          break;
        case UNIFORM:
          if(stepper_type.motor_position >= s_speed_parameter.start_decelerate_step)
          {
            stepper_type.motor_status = DECEL;
          }
          break;
      }
      /* ����λ���������� */
      stepper_type.motor_position++;
    }
    /* ��ȡ��ǰ��������ֵ */
		uint32_t tim_count = __HAL_TIM_GET_COUNTER(&TIM_TimeBaseStructure);
		/* ������һ��ʱ�� */
		uint16_t tmp = tim_count + stepper_type.pluse_time;
		/* ���ñȽ�ֵ */
		__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x, tmp);
  }
}


/**
  * @brief  �������S���߼Ӽ���
  * @param  axis��               ��Ҫ���Ƶ�����
  * @param  motor_start_speed��  �����ٶȣ���λ��ת/����
	* @param  motor_end_speed��    Ŀ���ٶȣ���λ��ת/����
	* @param  acceleration_time��  ����ʱ�䣬��λ����
	* @param  motor_move_step��    �˶���������λ�������迼��ϸ�֣�
  * @retval true��               ����
  * @retval false��              �������ô�����ٶȱ�ռ䲻��
	*	@note   ��
  */
bool Stepper_Move_SType(int16_t motor_start_speed, int16_t motor_end_speed, float acceleration_time, int32_t motor_move_step)
{
  /* �ж��Ƿ������˶� */
  if(stepper_type.motor_status != STOP)
  {
    return false;
  }    
  
  /* �����ٶȲ��� */
  if(CalcSpeed(motor_start_speed, motor_end_speed, acceleration_time) != true)
    return false;
  
  if(motor_move_step < 0)
  {
    motor_move_step = -motor_move_step;
    MOTOR_DIR(ANTI_CLOCKWISE);
  }else{
    MOTOR_DIR(CLOCKWISE);
  }
  
  /* ������ٵ㣬���˶�����С���������ٶ�ʱ�޷����s�Ӽ��� */
  if(motor_move_step >= s_speed_parameter.acclerate_total_step * 2)
  {
    s_speed_parameter.sum_total_step = motor_move_step;
    s_speed_parameter.start_decelerate_step = s_speed_parameter.sum_total_step - s_speed_parameter.acclerate_total_step;
  }else{
    return false;
  }
  
  /* ��ʼ���ṹ�� */
  memset(&stepper_type, 0, sizeof(STEPPER_TYPEDEF));
  /* ��ʼ�����״̬ */
	stepper_type.motor_status = ACCEL;
	stepper_type.motor_position = 0;
  /* �����һ���Ķ�ʱ������ */
  stepper_type.pluse_time = (uint16_t)(T1_FREQ / s_speed_parameter.motor_speed_form[0] / 2U);
  /* ��������� */
  __HAL_TIM_SET_COUNTER(&TIM_TimeBaseStructure, 0);
  __HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x, stepper_type.pluse_time);
  /* ʹ�ܶ�ʱ��ͨ�� */
	TIM_CCxChannelCmd(MOTOR_PUL_TIM, MOTOR_PUL_CHANNEL_x, TIM_CCx_ENABLE);
	/* ��������Ƚ��ж� */
	HAL_TIM_OC_Start_IT(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x);
  
  return true;
}


#endif 

