/********************************************************

				Ƕ��ʽ����ƽ̨�����жϳ�ʼ������

  -------------------------------------------------------
   �� �� ��   : bsp_stepper_T_speed.c
   �� �� ��   : V1.0.0
   ��    ��   : zp
   ��������   : 2020��09��07��	V1.0.0	zp
   �޶�����   ��2022��11��02��	V1.0.1	zp	����淶��
   ��������   : �жϳ�ʼ������
   ������     : hal��
   ע            
 ********************************************************/
  
#include "bsp_exti.h"
#include "bsp_led.h"   
#include "bsp_stepper_init.h"

 /**
  * @brief  ���� PA0 Ϊ���жϿڣ��������ж����ȼ�
  * @param  ��
  * @retval ��
  */
void EXTI_Key_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    /*��������GPIO�ڵ�ʱ��*/
    KEY1_INT_GPIO_CLK_ENABLE();
    KEY2_INT_GPIO_CLK_ENABLE();

    /* ѡ�񰴼�1������ */ 
    GPIO_InitStructure.Pin = KEY1_INT_GPIO_PIN;
    /* ��������Ϊ����ģʽ */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    		
    /* �������Ų�����Ҳ������ */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* ʹ������Ľṹ���ʼ������ */
    HAL_GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure); 
    /* ���� EXTI �ж�Դ ��key1 ���š������ж����ȼ�*/
    HAL_NVIC_SetPriority(KEY1_INT_EXTI_IRQ, 1, 1);
    /* ʹ���ж� */
    HAL_NVIC_EnableIRQ(KEY1_INT_EXTI_IRQ);

    /* ѡ�񰴼�2������ */ 
    GPIO_InitStructure.Pin = KEY2_INT_GPIO_PIN;  
    /* ����������������ͬ */
    HAL_GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStructure);      
    /* ���� EXTI �ж�Դ ��key1 ���š������ж����ȼ�*/
    HAL_NVIC_SetPriority(KEY2_INT_EXTI_IRQ, 1, 1);
    /* ʹ���ж� */
    HAL_NVIC_EnableIRQ(KEY2_INT_EXTI_IRQ);
}


int i=0,j=0;
int dir_val=0;
int en_val=0;

void KEY1_IRQHandler(void)
{
  /* ȷ���Ƿ������EXTI Line�ж� */
	if(__HAL_GPIO_EXTI_GET_IT(KEY1_INT_GPIO_PIN) != RESET) 
	{

		
    /* ����жϱ�־λ */
		__HAL_GPIO_EXTI_CLEAR_IT(KEY1_INT_GPIO_PIN);     
	}  
}

void KEY2_IRQHandler(void)
{
  /* ȷ���Ƿ������EXTI Line�ж� */
	if(__HAL_GPIO_EXTI_GET_IT(KEY2_INT_GPIO_PIN) != RESET) 
	{

    /* ����жϱ�־λ */
		__HAL_GPIO_EXTI_CLEAR_IT(KEY2_INT_GPIO_PIN);     
	}  
}

/*********************************************END OF FILE**********************/
