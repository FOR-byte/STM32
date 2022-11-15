/********************************************************

				嵌入式开发平台——led初始化函数

  -------------------------------------------------------
   文 件 名   : bsp_stepper_T_speed.c
   版 本 号   : V1.0.0
   作    者   : zp
   生成日期   : 2020年09月07日	V1.0.0	zp
   修订日期   ：2022年11月02日	V1.0.1	zp	代码规范化
   功能描述   : led初始化函数
   依赖于     : hal库
   注            
 ********************************************************/
  
#include "bsp_led.h"   

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void LED_GPIO_Config(void)
{		
		
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef  GPIO_InitStruct;

    /*开启LED相关的GPIO外设时钟*/
    LED1_GPIO_CLK_ENABLE();
    LED2_GPIO_CLK_ENABLE();
    LED3_GPIO_CLK_ENABLE();
  
    /*选择要控制的GPIO引脚*/															   
    GPIO_InitStruct.Pin = LED1_PIN;	

    /*设置引脚的输出类型为推挽输出*/
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  

    /*设置引脚为上拉模式*/
    GPIO_InitStruct.Pull  = GPIO_PULLUP;

    /*设置引脚速率为高速 */   
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH; 

    /*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
    HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);	

    /*选择要控制的GPIO引脚*/															   
    GPIO_InitStruct.Pin = LED2_PIN;	
    HAL_GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStruct);	

    /*选择要控制的GPIO引脚*/															   
    GPIO_InitStruct.Pin = LED3_PIN;	
    HAL_GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStruct);	

	//LED_ALL(LED_OFF);

}
/*********************************************END OF FILE**********************/


