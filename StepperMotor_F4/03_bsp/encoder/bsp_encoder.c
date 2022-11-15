/********************************************************

				Ƕ��ʽ����ƽ̨���������ʼ������

  -------------------------------------------------------
   �� �� ��   : bsp_encoder.c
   �� �� ��   : V1.0.0
   ��    ��   : zp
   ��������   : 2022��10��30��	V1.0.0	zp
   �޶�����   ��2022��11��01��	V1.0.1	zp	����淶��
   ��������   : ��������ʼ������
   ������     : 
   ע            
 ********************************************************/

#include "bsp_encoder.h"

/* ��ʱ��������� */
__IO int16_t encoder_overflow_count = 0;

TIM_HandleTypeDef TIM_EncoderHandle;

/**
  * @brief  �������ӿ����ų�ʼ��
  * @param  ��
  * @retval ��
  */
static void Encoder_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  /* ��ʱ��ͨ�����Ŷ˿�ʱ��ʹ�� */
  ENCODER_TIM_CH1_GPIO_CLK_ENABLE();
  ENCODER_TIM_CH2_GPIO_CLK_ENABLE();
  
  /* ������������ */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  /* �������� */
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  /* ������������ */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  
  /* ѡ��Ҫ���Ƶ�GPIO���� */	
  GPIO_InitStruct.Pin = ENCODER_TIM_CH1_PIN;
  /* ���ø��� */
  GPIO_InitStruct.Alternate = ENCODER_TIM_CH1_GPIO_AF;
  /* ���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO */
  HAL_GPIO_Init(ENCODER_TIM_CH1_GPIO_PORT, &GPIO_InitStruct);
  
  /* ѡ��Ҫ���Ƶ�GPIO���� */	
  GPIO_InitStruct.Pin = ENCODER_TIM_CH2_PIN;
  /* ���ø��� */
  GPIO_InitStruct.Alternate = ENCODER_TIM_CH2_GPIO_AF;
  /* ���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO */
  HAL_GPIO_Init(ENCODER_TIM_CH2_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  ����TIMx������ģʽ
  * @param  ��
  * @retval ��
  */
static void TIM_Encoder_Init(void)
{ 
  TIM_Encoder_InitTypeDef Encoder_ConfigStructure;
  
  /* ʹ�ܱ������ӿ�ʱ�� */
  ENCODER_TIM_CLK_ENABLE();
  
  /* ��ʱ����ʼ������ */
  TIM_EncoderHandle.Instance = ENCODER_TIM;
  TIM_EncoderHandle.Init.Prescaler = ENCODER_TIM_PRESCALER;
  TIM_EncoderHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  TIM_EncoderHandle.Init.Period = ENCODER_TIM_PERIOD;
  TIM_EncoderHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  TIM_EncoderHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  
  /* ���ñ�������Ƶ�� */
  Encoder_ConfigStructure.EncoderMode = ENCODER_MODE;
  /* �������ӿ�ͨ��1���� */
  Encoder_ConfigStructure.IC1Polarity = ENCODER_IC1_POLARITY;
  Encoder_ConfigStructure.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  Encoder_ConfigStructure.IC1Prescaler = TIM_ICPSC_DIV1;
  Encoder_ConfigStructure.IC1Filter = 0;
  /* �������ӿ�ͨ��2���� */
  Encoder_ConfigStructure.IC2Polarity = ENCODER_IC2_POLARITY;
  Encoder_ConfigStructure.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  Encoder_ConfigStructure.IC2Prescaler = TIM_ICPSC_DIV1;
  Encoder_ConfigStructure.IC2Filter = 0;
  /* ��ʼ���������ӿ� */
  HAL_TIM_Encoder_Init(&TIM_EncoderHandle, &Encoder_ConfigStructure);
  
  /* ��������� */
  __HAL_TIM_SET_COUNTER(&TIM_EncoderHandle, 0);
  
  /* �����жϱ�־λ */
  __HAL_TIM_CLEAR_IT(&TIM_EncoderHandle,TIM_IT_UPDATE);
  /* ʹ�ܶ�ʱ���ĸ����¼��ж� */
  __HAL_TIM_ENABLE_IT(&TIM_EncoderHandle,TIM_IT_UPDATE);
  /* ���ø����¼�����ԴΪ����������� */
  __HAL_TIM_URS_ENABLE(&TIM_EncoderHandle);
  
  /* �����ж����ȼ� */
  HAL_NVIC_SetPriority(ENCODER_TIM_IRQn, 5, 1);
  /* ʹ�ܶ�ʱ���ж� */
  HAL_NVIC_EnableIRQ(ENCODER_TIM_IRQn);
  
  /* ʹ�ܱ������ӿ� */
  HAL_TIM_Encoder_Start(&TIM_EncoderHandle, TIM_CHANNEL_ALL);
}

/**
  * @brief  �������ӿڳ�ʼ��
  * @param  ��
  * @retval ��
  */
void Encoder_Init(void)
{
  Encoder_GPIO_Init();    /* ���ų�ʼ�� */
  TIM_Encoder_Init();     /* ���ñ������ӿ� */
}


/**
  * @brief  ��ʱ�������¼��ص�����
  * @param  ��
  * @retval ��
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* �жϵ�ǰ�������������� */
  if(__HAL_TIM_IS_TIM_COUNTING_DOWN(htim))
    /* ���� */
    encoder_overflow_count--;
  else
    /* ���� */
    encoder_overflow_count++;
}

/*********************************************END OF FILE**********************/
