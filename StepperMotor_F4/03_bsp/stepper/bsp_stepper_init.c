/********************************************************

				Ƕ��ʽ����ƽ̨���������ʼ������

  -------------------------------------------------------
   �� �� ��   : bsp_stepper_init.c
   �� �� ��   : V1.0.0
   ��    ��   : zp
   ��������   : 2022��10��30��	V1.0.0	zp
   �޶�����   ��2022��11��01��	V1.0.1	zp	����淶��
   ��������   : �����ʼ������
   ������     : 
   ע            
 ********************************************************/ 
 
#include "bsp_stepper_init.h"
#include "bsp_delay.h"

TIM_HandleTypeDef TIM_TimeBaseStructure;

/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */

static void Stepper_GPIO_Config(void) 
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/*����Motor��ص�GPIO����ʱ��*/
	MOTOR_DIR_GPIO_CLK_ENABLE();
	MOTOR_PUL_GPIO_CLK_ENABLE();
	MOTOR_EN_GPIO_CLK_ENABLE();

	/*ѡ��Ҫ���Ƶ�GPIO����*/															   
	GPIO_InitStruct.Pin = MOTOR_DIR_PIN;	
	/*�������ŵ��������Ϊ�������*/
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  
	GPIO_InitStruct.Pull =GPIO_PULLUP;
	/*������������Ϊ���� */   
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	/*Motor �������� ��ʼ��*/
	HAL_GPIO_Init(MOTOR_DIR_GPIO_PORT, &GPIO_InitStruct);	

	/*Motor ʹ������ ��ʼ��*/
	//GPIO_InitStruct.Pin = MOTOR_EN_PIN;	
	HAL_GPIO_Init(MOTOR_EN_GPIO_PORT, &GPIO_InitStruct);	


	/* ��ʱ��ͨ��1��������IO��ʼ�� */
	/*�����������*/
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	/*������������ */ 
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	/*���ø���*/
	GPIO_InitStruct.Alternate = MOTOR_PUL_GPIO_AF;
	/*���ø���*/
	GPIO_InitStruct.Pull =GPIO_PULLUP;
	/*ѡ��Ҫ���Ƶ�GPIO����*/	
	GPIO_InitStruct.Pin = MOTOR_PUL_PIN;
	/*Motor �������� ��ʼ��*/
	HAL_GPIO_Init(MOTOR_PUL_PORT, &GPIO_InitStruct);			
}

 /**
  * @brief  �ж����ȼ�����
  * @param  ��
  * @retval ��
  */
static void TIMx_NVIC_Configuration(void)
{
	/* �����ж����� */
	HAL_NVIC_SetPriority(MOTOR_PUL_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(MOTOR_PUL_IRQn);
}

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode			 TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */
void TIM_PWMOUTPUT_Config(void)
{
	TIM_OC_InitTypeDef  TIM_OCInitStructure;  	
	/* ʹ�ܶ�ʱ�� */
	MOTOR_PUL_CLK_ENABLE();

	TIM_TimeBaseStructure.Instance = MOTOR_PUL_TIM;    
	/* �ۼ� TIM_Period�������һ�����»����ж� */		
	//����ʱ����0������10000����Ϊ10000�Σ�Ϊһ����ʱ����
	TIM_TimeBaseStructure.Init.Period = TIM_PERIOD; 
	// ͨ�ÿ��ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2	=	84MHz 
	// �߼����ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK		=	168MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)
	TIM_TimeBaseStructure.Init.Prescaler = TIM_PRESCALER;                

	/* ������ʽ */
	TIM_TimeBaseStructure.Init.CounterMode = TIM_COUNTERMODE_UP;            
	/* ����ʱ�ӷ�Ƶ */	
	TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;   
	TIM_TimeBaseStructure.Init.RepetitionCounter = 0 ;  		
	/* ��ʼ����ʱ�� */
	HAL_TIM_OC_Init(&TIM_TimeBaseStructure);

	/* PWMģʽ����--��������Ϊ����Ƚ�ģʽ */
	TIM_OCInitStructure.OCMode = TIM_OCMODE_TOGGLE; 
	/* �Ƚ�����ļ���ֵ */
	TIM_OCInitStructure.Pulse = 0;                    
	/* ����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ */
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;          
	/* ���û���ͨ������ļ��� */
	TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_LOW; 
	/* ����ģʽ���� */
	TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;   
	/* ���е�ƽ */
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_RESET;  
	/* ����ͨ������ */
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	HAL_TIM_OC_ConfigChannel(&TIM_TimeBaseStructure, &TIM_OCInitStructure, MOTOR_PUL_CHANNEL_x);
	HAL_TIM_Base_Stop(&TIM_TimeBaseStructure);
}


/**
  * @brief  ���ų�ʼ��
  * @retval ��
  */
void Stepper_Init(void)
{
	/* ���IO���� */
	Stepper_GPIO_Config();
	/* ��ʱ��PWM������� */
	TIM_PWMOUTPUT_Config();
	/* �ж����� */
	TIMx_NVIC_Configuration();
}


/**
  * @brief  ��ʱ���жϷ�����
  * @retval ��
  */
void MOTOR_PUL_IRQHandler(void)
{ 
  if(SPEED_CURVE == 1)
  {
    /* T�� �ٶ�״̬���� */
    TSpeed_Decision();
  }else if(SPEED_CURVE == 2){
    /* S�� �ٶ�״̬���� */
    Speed_Decision();
  }
}
