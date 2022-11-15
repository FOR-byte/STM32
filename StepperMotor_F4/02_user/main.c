/********************************************************

				Ƕ��ʽ����ƽ̨����Main����

  -------------------------------------------------------
   �� �� ��   : main.c
   �� �� ��   : V1.0.0
   ��    ��   : zp
   ��������   : 2020��09��07��	V1.0.0	zp
   �޶�����   ��2022��11��02��	V1.0.0	zp	����淶��
   ��������   : ����main������һ����ʽ��
   ������     :
   ע            
 ********************************************************/

/* Includes ------------------------------------------- */
#include "main.h"
#include "stm32f4xx.h"
#include "bsp_delay.h"
#include "bsp_exti.h"
#include "bsp_led.h"
#include "bsp_debug_usart.h"
#include "bsp_stepper_init.h"
#include "bsp_encoder.h"
#include "drv_stepper_T_speed.h"
#include "drv_stepper_S_speed.h"
#include "drv_stepper_parameter.h"


///* Ӳ�������ٶȵ����ޣ�����㷨�����Ƿ���Դﵽ���� */
///* �ٶ� ��λΪ0.1rad/sec */
//__IO uint32_t set_motor_speed  =1000;

///* ���ٶȺͼ��ٶ�ѡȡһ�����ʵ����Ҫ��ֵԽ���ٶȱ仯Խ�죬�Ӽ��ٽ׶αȽ϶��������Լ��ٶȺͼ��ٶ�ֵһ������ʵ��Ӧ���жೢ�Գ����Ľ�� */

///* ���ٶȵ�λΪ 0.1rad/sec^2 */
//__IO uint32_t acceleration_value = 100;
///* ���ٶȵ�λΪ 0.1rad/sec^2 */ 
//__IO uint32_t deceleration_value = 100;

///* �����ת���� */
//__IO int8_t motor_direction = 0;
///* ��ǰʱ���ܼ���ֵ */
//__IO int32_t capture_count = 0;
///* ��һʱ���ܼ���ֵ */
//__IO int32_t last_count = 0;
///* ��λʱ�����ܼ���ֵ */
//__IO int32_t count_per_unit = 0;
///* ���ת��ת�� */
//__IO float shaft_speed = 0.0f;
///* �ۻ�Ȧ�� */
//__IO float number_of_rotations = 0.0f;


/**
  * @brief  main
  * @param  
  * @retval 
  */
int main(void) 
{
//  HAL_Init();
//  HAL_InitTick(0);	
	/* ��ʼ��ϵͳʱ��Ϊ168MHz */
	SystemClock_Config();
	/* ��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ��� */
	Debug_Usart_Config();
  /* ��������ʼ�� */
  Encoder_Init();
	/* ���������ʼ�� */
	Stepper_Init();

	while(1)
	{
    /* processing serial port data */
//    Stepper_Move_SType(60, 1784, 0.1f, 5 * SPR);
//    while(stepper_type.motor_status != STOP);
//    HAL_Delay(1000);
//    dir_value=(++motor_turnover_flag % 2) ? MOTOR_ROTATES_CLOCKWISE : MOTOR_ROTATES_COUNTERCLOCKWISE;
//    //printf("dir_val =%d\r\n",dir_val);
//    if(dir_value)
//    {
//      Stepper_Move_Trapezoid(3200*5, acceleration_value, deceleration_value, set_motor_speed);
//      HAL_Delay(2000);
//    }
//    else 
//    {
//      Stepper_Move_Trapezoid(-3200*10, acceleration_value, deceleration_value, set_motor_speed);
//    }		
	}
} 	

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
 void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1) {};
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    while(1) {};
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/



