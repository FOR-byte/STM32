/********************************************************

				嵌入式开发平台——电机S形算法函数

  -------------------------------------------------------
   文 件 名   : bsp_stepper_S_speed.c
   版 本 号   : V1.0.0
   作    者   : zp
   生成日期   : 2022年11月03日	V1.0.0	zp
   修订日期   : 2022年11月04日	V1.0.1	zp
   功能描述   : 电机S型算法实现函数
   依赖于     : 
   注            
 ********************************************************/
 
#include "drv_stepper_S_speed.h"
#include "drv_stepper_parameter.h"

#if(SPEED_CURVE == 2)

/* 算法相关结构体变量定义 */
SPEEDCALC_TYPEDEF s_speed_parameter;
STEPPER_TYPEDEF stepper_type = {0};

static bool CalcSpeed(int32_t motor_start_speed, int32_t motor_end_speed, float motor_acclerate_time)
{
  int32_t pulse_period_count = 0;           /* 脉冲数标志位 */  
  float midpoint_speed =0.0f;               /* 中间点速度 */
  float double_acceleration = 0.0f;         /* 加加速度 */
  float time_interval = 0.0f;               /* 时间间隔 */
  float time_accumulation = 0.0f;           /* 时间累加量 */  
  float speed_increment = 0.0f;             /* 速度的增量 */
  float temporary_variable = 0.0f;          /* 中间变量 */                
  
  /* 计算初速度、末速度 */
  s_speed_parameter.motor_start_speed = CONVER(motor_start_speed);
  s_speed_parameter.motor_end_speed = CONVER(motor_end_speed);
  
  /* 计算初始参数 */
  
  /* 加加速段的时间（加速度斜率大于0的时间） */
  motor_acclerate_time = motor_acclerate_time / 2.0f;                 
  /* 计算中点的速度 */  
  midpoint_speed = (s_speed_parameter.motor_start_speed + s_speed_parameter.motor_end_speed) / 2.0f;    
  /* 根据中点速度计算加加速度 */
  double_acceleration = fabsf((2.0f * (midpoint_speed - s_speed_parameter.motor_start_speed)) / (motor_acclerate_time * motor_acclerate_time));  
  /* 加加速需要的步数 */ 
  s_speed_parameter.inc_acclerate_total_step = (int32_t)((s_speed_parameter.motor_start_speed * motor_acclerate_time) + ((double_acceleration * motor_acclerate_time * motor_acclerate_time * motor_acclerate_time) / 6.0f));
  /* 减加速需要的步数 */
  s_speed_parameter.dec_acclerate_total_step = (int32_t)(((s_speed_parameter.motor_end_speed + s_speed_parameter.motor_start_speed) * motor_acclerate_time - s_speed_parameter.inc_acclerate_total_step));
  /* 计算总共需要的步数 */
  s_speed_parameter.acclerate_total_step = s_speed_parameter.inc_acclerate_total_step + s_speed_parameter.dec_acclerate_total_step;
  if(s_speed_parameter.acclerate_total_step % 2 != 0 )
    s_speed_parameter.acclerate_total_step += 1;
  
  /* 判断内存大小 */
  if(FORM_LEN <s_speed_parameter.acclerate_total_step)
  {
    return false;
  }
  
  /* 计算第一步的时间 */
	/* 根据第一步的时间计算，第一步的速度和脉冲时间间隔 */
	/* 根据位移为0的时候的情况，计算时间的关系式 -> 根据位移和时间的公式S = 1/2 * K * Ti^3  可得 Ti=6 * 1 / K开1/3次方 */
  
  /* 开发求解时间常数 */
  time_interval = pow(6.0f *1.0f / double_acceleration,1.0f / 3.0f);
  /* 累计时间常数 */
  time_accumulation += time_interval; 
  /* 根据V=1/2*K*T^2,可以计算第一步的速度 */
  speed_increment = 0.5f * double_acceleration * time_accumulation * time_accumulation;
  /* 在初始速度下增加加速度 */
  s_speed_parameter.motor_speed_form[0] = s_speed_parameter.motor_start_speed + speed_increment;
  /* 最小速度限幅 */  
  /* 以当前定时器频率所能到达的最低速度 */
  if(s_speed_parameter.motor_speed_form[0] <= MIN_SPEED)
  {
    s_speed_parameter.motor_speed_form[0] = MIN_SPEED;
  }
  /* 计算S型速度表 */
  for(pulse_period_count = 1;pulse_period_count < s_speed_parameter.acclerate_total_step;pulse_period_count++)
  {
    /* 根据时间周期与频率成反比的关系，可以计算出Ti,在这里每次计算上一步时间，用于积累到当前时间 */
    time_interval = 1.0f / s_speed_parameter.motor_speed_form[pulse_period_count-1];
    if(pulse_period_count < s_speed_parameter.inc_acclerate_total_step)
    {
			/* 累积时间 */
      time_accumulation += time_interval;
			/* 速度的变化量 dV = 1/2 * K * Ti^2 */
      speed_increment = 0.5f * double_acceleration * time_accumulation * time_accumulation;
			/* 根据初始速度和变化量求得速度表 */
      s_speed_parameter.motor_speed_form[pulse_period_count] = s_speed_parameter.motor_start_speed + speed_increment;
			/* 为了保证在最后一步可以使得时间严谨的与预期计算的时间一致，在最后一步进行处理 */
      if(pulse_period_count == s_speed_parameter.inc_acclerate_total_step - 1)
        {time_accumulation  = fabsf(time_accumulation - motor_acclerate_time);}
    }
    /* 减加速度计算 */
    else
    {
			/* 时间累积 */
      time_accumulation += time_interval;
			/* 计算速度 */
      temporary_variable = fabsf(motor_acclerate_time - time_accumulation);
      speed_increment = 0.5f * double_acceleration * temporary_variable * temporary_variable;
      s_speed_parameter.motor_speed_form[pulse_period_count] = s_speed_parameter.motor_end_speed - speed_increment;
    }
  }
  return true;  
}  

/**
  * @brief  速度决策
  * @param  pScurve：S曲线结构体指针
  * @retval 无
	*	@note 	在中断中使用，每进一次中断，决策一次
  */
void Speed_Decision(void)
{
  /* 脉冲计数 */
  static __IO uint8_t i = 0;
  static __IO uint32_t index = 0;
  
  if(__HAL_TIM_GET_IT_SOURCE(&TIM_TimeBaseStructure, MOTOR_TIM_IT_CCx) != RESET)
  {
    /* 清除定时器中断 */
		__HAL_TIM_CLEAR_IT(&TIM_TimeBaseStructure, MOTOR_TIM_IT_CCx);
    /* 两次脉冲为一个周期 */
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
          /* 获取每一步的定时器计数值 */
          stepper_type.pluse_time = (uint16_t)(T1_FREQ / s_speed_parameter.motor_speed_form[index] / 2U);
          /* 步数位置索引递增 */
          index++;
          break;
        case DECEL:
          if(stepper_type.motor_position >= (s_speed_parameter.sum_total_step - 1))
          {
            /* 进入停止状态，清空速度表并且关闭输出通道 */
            HAL_TIM_OC_Stop_IT(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x);
            memset((void*)s_speed_parameter.motor_speed_form, 0, sizeof(float) * FORM_LEN);
            index = 0;
            stepper_type.motor_status = STOP;
            break;
          }
          /* 获取每一步的定时器计数值 */
          stepper_type.pluse_time = (uint16_t)(T1_FREQ / s_speed_parameter.motor_speed_form[index] / 2U);
          /* 步数位置索引递增 */
          index--;
          break;
        case UNIFORM:
          if(stepper_type.motor_position >= s_speed_parameter.start_decelerate_step)
          {
            stepper_type.motor_status = DECEL;
          }
          break;
      }
      /* 步数位置索引递增 */
      stepper_type.motor_position++;
    }
    /* 获取当前计数器数值 */
		uint32_t tim_count = __HAL_TIM_GET_COUNTER(&TIM_TimeBaseStructure);
		/* 计算下一次时间 */
		uint16_t tmp = tim_count + stepper_type.pluse_time;
		/* 设置比较值 */
		__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x, tmp);
  }
}


/**
  * @brief  步进电机S曲线加减速
  * @param  axis：               想要控制的轴编号
  * @param  motor_start_speed：  启动速度，单位：转/分钟
	* @param  motor_end_speed：    目标速度，单位：转/分钟
	* @param  acceleration_time：  加速时间，单位：秒
	* @param  motor_move_step：    运动步数，单位：步（需考虑细分）
  * @retval true：               正常
  * @retval false：              参数设置错误或速度表空间不足
	*	@note   无
  */
bool Stepper_Move_SType(int16_t motor_start_speed, int16_t motor_end_speed, float acceleration_time, int32_t motor_move_step)
{
  /* 判断是否正在运动 */
  if(stepper_type.motor_status != STOP)
  {
    return false;
  }    
  
  /* 计算速度参数 */
  if(CalcSpeed(motor_start_speed, motor_end_speed, acceleration_time) != true)
    return false;
  
  if(motor_move_step < 0)
  {
    motor_move_step = -motor_move_step;
    MOTOR_DIR(ANTI_CLOCKWISE);
  }else{
    MOTOR_DIR(CLOCKWISE);
  }
  
  /* 计算减速点，当运动步数小于两倍加速段时无法完成s加减速 */
  if(motor_move_step >= s_speed_parameter.acclerate_total_step * 2)
  {
    s_speed_parameter.sum_total_step = motor_move_step;
    s_speed_parameter.start_decelerate_step = s_speed_parameter.sum_total_step - s_speed_parameter.acclerate_total_step;
  }else{
    return false;
  }
  
  /* 初始化结构体 */
  memset(&stepper_type, 0, sizeof(STEPPER_TYPEDEF));
  /* 初始化电机状态 */
	stepper_type.motor_status = ACCEL;
	stepper_type.motor_position = 0;
  /* 计算第一步的定时器参数 */
  stepper_type.pluse_time = (uint16_t)(T1_FREQ / s_speed_parameter.motor_speed_form[0] / 2U);
  /* 清零计数器 */
  __HAL_TIM_SET_COUNTER(&TIM_TimeBaseStructure, 0);
  __HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x, stepper_type.pluse_time);
  /* 使能定时器通道 */
	TIM_CCxChannelCmd(MOTOR_PUL_TIM, MOTOR_PUL_CHANNEL_x, TIM_CCx_ENABLE);
	/* 开启输出比较中断 */
	HAL_TIM_OC_Start_IT(&TIM_TimeBaseStructure, MOTOR_PUL_CHANNEL_x);
  
  return true;
}


#endif 

