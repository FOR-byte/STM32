/********************************************************

				嵌入式开发平台——中断初始化函数

  -------------------------------------------------------
   文 件 名   : bsp_stepper_T_speed.c
   版 本 号   : V1.0.0
   作    者   : zp
   生成日期   : 2020年09月07日	V1.0.0	zp
   修订日期   ：2022年11月02日	V1.0.1	zp	代码规范化
   功能描述   : 中断初始化函数
   依赖于     : hal库
   注            
 ********************************************************/
  
#include "bsp_exti.h"
#include "bsp_led.h"   
#include "bsp_stepper_init.h"

 /**
  * @brief  配置 PA0 为线中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
void EXTI_Key_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    /*开启按键GPIO口的时钟*/
    KEY1_INT_GPIO_CLK_ENABLE();
    KEY2_INT_GPIO_CLK_ENABLE();

    /* 选择按键1的引脚 */ 
    GPIO_InitStructure.Pin = KEY1_INT_GPIO_PIN;
    /* 设置引脚为输入模式 */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    		
    /* 设置引脚不上拉也不下拉 */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* 使用上面的结构体初始化按键 */
    HAL_GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure); 
    /* 配置 EXTI 中断源 到key1 引脚、配置中断优先级*/
    HAL_NVIC_SetPriority(KEY1_INT_EXTI_IRQ, 1, 1);
    /* 使能中断 */
    HAL_NVIC_EnableIRQ(KEY1_INT_EXTI_IRQ);

    /* 选择按键2的引脚 */ 
    GPIO_InitStructure.Pin = KEY2_INT_GPIO_PIN;  
    /* 其他配置与上面相同 */
    HAL_GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStructure);      
    /* 配置 EXTI 中断源 到key1 引脚、配置中断优先级*/
    HAL_NVIC_SetPriority(KEY2_INT_EXTI_IRQ, 1, 1);
    /* 使能中断 */
    HAL_NVIC_EnableIRQ(KEY2_INT_EXTI_IRQ);
}


int i=0,j=0;
int dir_val=0;
int en_val=0;

void KEY1_IRQHandler(void)
{
  /* 确保是否产生了EXTI Line中断 */
	if(__HAL_GPIO_EXTI_GET_IT(KEY1_INT_GPIO_PIN) != RESET) 
	{

		
    /* 清除中断标志位 */
		__HAL_GPIO_EXTI_CLEAR_IT(KEY1_INT_GPIO_PIN);     
	}  
}

void KEY2_IRQHandler(void)
{
  /* 确保是否产生了EXTI Line中断 */
	if(__HAL_GPIO_EXTI_GET_IT(KEY2_INT_GPIO_PIN) != RESET) 
	{

    /* 清除中断标志位 */
		__HAL_GPIO_EXTI_CLEAR_IT(KEY2_INT_GPIO_PIN);     
	}  
}

/*********************************************END OF FILE**********************/
