/********************************************************

				嵌入式开发平台——电机梯形算法函数

  -------------------------------------------------------
   文 件 名   : bsp_stepper_T_speed.c
   版 本 号   : V1.0.0
   作    者   : zp
   生成日期   : 2022年10月30日	V1.0.0	zp
   修订日期   : 2022年11月02日	V1.0.1	zp	代码规范化
   功能描述   : 电机梯形算法实现函数
   依赖于     : hal库
   注            
 ********************************************************/
 
#include "drv_stepper_T_speed.h"
#include "drv_stepper_parameter.h"

#if(SPEED_CURVE == 1)

/* 系统加减速参数 */
SPEED_RAMP_TYPE g_motor_parameter;

/*
    记录步进电机位置
    系统电机，串口状态  */

struct GLOBAL_FLAGS motor_status = {FALSE, FALSE,TRUE};



/*! \brief 给定步数移动电机
 *  通过计算加速到最大速度，以给定的步数开始减速
 *  如果加速度和减速度很小，步进电机会移动很慢，还没达到最大速度就要开始减速
 *
 *  \param move_step     移动的步数 (正数为顺时针，负数为逆时针)
 *  \param acceleration  加速度,如果取值为10，   实际值为 10 * 0.1 * rad / sec^2 = 1rad / sec^2
 *  \param deceleration  减速度,如果取值为10，   实际值为 10 * 0.1 * rad / sec^2 = 1rad / sec^2
 *  \param max_speed     最大速度,如果取值为10， 实际值为 10 * 0.1 * rad / sec = 1rad / sec
 */
void Stepper_Move_Trapezoid( int32_t move_step, uint32_t acceleration, uint32_t deceleration, uint32_t max_speed)
{  
    /* 达到最大速度时的步数 */
    unsigned int max_speed_steps;
    /* 必须开始减速的步数（如果还没加速到达最大速度时） */
    unsigned int acceleration_limit_steps;

    /* 根据步数和正负判断 */
    if(move_step == 0)
    {
          /* 如果步数为零，退出循环 */
      return ;
    }
    else if(move_step < 0)
    {
        /* 设置电机旋转方向为逆时针 */
        g_motor_parameter.motor_rotate_dir = MOTOR_ROTATES_COUNTERCLOCKWISE;
        move_step = -move_step;
    }
    else
    {
        g_motor_parameter.motor_rotate_dir = MOTOR_ROTATES_CLOCKWISE;
    }	
    /* 输出电机旋转方向 */
    MOTOR_DIR(g_motor_parameter.motor_rotate_dir);
		    
    if(move_step == 1)
    {
        g_motor_parameter.accelerate_steps = -1;
        /* 减速状态 */ 
        g_motor_parameter.motor_run_state = DECEL;
        /* 短延时 */ 
        g_motor_parameter.pulse_interval = 1000;
        /* 配置电机运行状态 */ 
        motor_status.running = TRUE;
    }
		
    /* 步数不为零才移动 */ 
    else if(move_step != 0)
    {
      /* 设置最大速度极限, 计算得到min_delay用于定时器的计数器的值   min_delay = (alpha / tt)/ w */
      g_motor_parameter.min_interval = (int32_t)(A_T_x10/max_speed);
      /* 通过计算第一个(c0) 的步进延时来设定加速度，acceleration位为 0.1rad/sec^2 */ 
      /* pulse_interval = 1/tt * sqrt(2*alpha/accel) */ 
      /* pulse_interval = ( tfreq*0.676/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100 */ 
      g_motor_parameter.pulse_interval = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / acceleration))/10);

      /* 计算多少步之后达到最大速度的限制  max_s_lim = speed^2 / (2*alpha*accel) */ 
      max_speed_steps = (uint32_t)(max_speed*max_speed/(A_x200*acceleration/10));
      /* 如果达到最大速度小于0.5步，我们将四舍五入为0，但实际我们必须移动至少一步才能达到想要的速度 */ 
      if(max_speed_steps == 0)
      {
        max_speed_steps = 1;
      }
      /* 计算多少步之后我们必须开始减速 n1 = (n1+n2)decel / (accel + decel) */ 
      acceleration_limit_steps = (uint32_t)(move_step*deceleration/(acceleration+deceleration));
      /* 我们必须加速至少1步才能才能开始减速 */ 
      if(acceleration_limit_steps == 0)
      {
        acceleration_limit_steps = 1;
      }
      /* 使用限制条件我们可以计算出第一次开始减速的位置 */ 
      if(acceleration_limit_steps <= max_speed_steps)
      {
        /* 结果为负值 */
        g_motor_parameter.decelerate_steps = acceleration_limit_steps - move_step;
      }
      else{
        /* 结果为负值 */
        g_motor_parameter.decelerate_steps = -(max_speed_steps*acceleration/deceleration);
      }
      /* 当只剩下一步我们必须减速 */ 
      if(g_motor_parameter.decelerate_steps == 0)
      {
        g_motor_parameter.decelerate_steps = -1;
      }

      /* 计算开始减速时的步数 */ 
      g_motor_parameter.decelerate_position = move_step + g_motor_parameter.decelerate_steps;

      /* 如果最大速度很慢，我们就不需要进行加速运动 */ 
      if(g_motor_parameter.pulse_interval <= g_motor_parameter.min_interval)
      {
        g_motor_parameter.pulse_interval = g_motor_parameter.min_interval;
        g_motor_parameter.motor_run_state = RUN;
      }
      else
      {
        g_motor_parameter.motor_run_state = ACCEL;
      }
      /* 复位加速度计数值 */ 
      g_motor_parameter.accelerate_steps = 0;
      motor_status.running = TRUE;
    }
    /* 获取当前计数值 */
    int tim_count=__HAL_TIM_GET_COUNTER(&TIM_TimeBaseStructure);
    /* 在当前计数值基础上设置定时器比较值 */
    __HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure,MOTOR_PUL_CHANNEL_x,tim_count+g_motor_parameter.pulse_interval/2); 
    /* 使能定时器通道 */
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
	/* 保存新（下）一个延时周期 */ 
	uint16_t new_step_delay=0;
	/* 加速过程中最后一次延时（脉冲周期） */ 
	static uint16_t last_accelerate_delay=0;
	/* 总移动步数计数器 */ 
	static uint32_t move_step_count = 0;
    /* 脉冲周期余数 */
	static int32_t pulse_cycle_remainder = 0;
	/* 定时器使用翻转模式，需要进入两次中断才输出一个完整脉冲 */
	static uint8_t interrupt_flag_count=0;
  
	if(__HAL_TIM_GET_IT_SOURCE(&TIM_TimeBaseStructure, MOTOR_TIM_IT_CCx) !=RESET)
	{
		/* 清除定时中断 */ 
		__HAL_TIM_CLEAR_IT(&TIM_TimeBaseStructure, MOTOR_TIM_IT_CCx);

		/* 设置比较值 */
		tim_count=__HAL_TIM_GET_COUNTER(&TIM_TimeBaseStructure);
		tmp = tim_count+g_motor_parameter.pulse_interval/2;
		__HAL_TIM_SET_COMPARE(&TIM_TimeBaseStructure,MOTOR_PUL_CHANNEL_x,tmp);

		interrupt_flag_count++;
		if(interrupt_flag_count==2) /* 2次，说明已经输出一个完整脉冲 */ 
		{
			interrupt_flag_count=0;
			switch(g_motor_parameter.motor_run_state) 
			{
				/*步进电机停止状态*/
				case STOP:
            move_step_count = 0;
            pulse_cycle_remainder = 0;
            /* 关闭通道 */ 
            HAL_TIM_OC_Stop_IT(&TIM_TimeBaseStructure,MOTOR_PUL_CHANNEL_x);
            motor_status.running = FALSE;
            break;
        /*步进电机加速状态*/
        case ACCEL:
            TIM_CCxChannelCmd(MOTOR_PUL_TIM, MOTOR_PUL_CHANNEL_x, TIM_CCx_ENABLE);                                         
            move_step_count++;
            g_motor_parameter.accelerate_steps++;
            /* 计算新（下）一步脉冲周期（时间间隔） */
            new_step_delay = g_motor_parameter.pulse_interval - (((2 *g_motor_parameter.pulse_interval) + pulse_cycle_remainder)/(4 * g_motor_parameter.accelerate_steps + 1));
            /* 计算余数，下次计算补上余数，减少误差 */ 
            pulse_cycle_remainder = ((2 * g_motor_parameter.pulse_interval)+pulse_cycle_remainder)%(4 * g_motor_parameter.accelerate_steps + 1);
            /* 检查是够应该开始减速 */
            if(move_step_count >= g_motor_parameter.decelerate_position) {
              g_motor_parameter.accelerate_steps = g_motor_parameter.decelerate_steps;
              g_motor_parameter.motor_run_state = DECEL;
            }
            /* 检查是否达到期望的最大速度 */
            else if(new_step_delay <= g_motor_parameter.min_interval) {
              last_accelerate_delay = new_step_delay;
              new_step_delay = g_motor_parameter.min_interval;    
              pulse_cycle_remainder = 0;                          
              g_motor_parameter.motor_run_state = RUN;
            }
            break;
				/* 步进电机最大速度运行状态 */
				case RUN:

            move_step_count++;
            new_step_delay = g_motor_parameter.min_interval;

            if(move_step_count >= g_motor_parameter.decelerate_position) 
            {
              g_motor_parameter.accelerate_steps = g_motor_parameter.decelerate_steps;
              /* 以最后一次加速的延时作为开始减速的延时 */
              new_step_delay = last_accelerate_delay;
              g_motor_parameter.motor_run_state = DECEL;
            }
            break;
				/* 步进电机减速状态 */
				case DECEL:
            move_step_count++;
            g_motor_parameter.accelerate_steps++;
            /* 计算新(下)一步脉冲周期(时间间隔) */
            new_step_delay = g_motor_parameter.pulse_interval - (((2 * g_motor_parameter.pulse_interval) + pulse_cycle_remainder)/(4 * g_motor_parameter.accelerate_steps + 1)); 
            /* 计算余数，下次计算补上余数，减少误差 */ 
            pulse_cycle_remainder = ((2 * g_motor_parameter.pulse_interval)+pulse_cycle_remainder)%(4 * g_motor_parameter.accelerate_steps + 1);
            /* 检查是否为最后一步 */
            if(g_motor_parameter.accelerate_steps >= 0)
            {
              g_motor_parameter.motor_run_state = STOP;
            }
            break;
			}
			/* 求得下一次间隔时间 */
			g_motor_parameter.pulse_interval = new_step_delay;
		}
	}
}

#endif


