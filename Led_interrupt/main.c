#include "stm32f4xx.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_tim.h"
volatile count=0;
void ini()
{

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIOInitStruct,gpio_input;
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIOInitStruct.GPIO_OType = GPIO_OType_PP;
	GPIOInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);
	gpio_input.GPIO_Mode = GPIO_Mode_IN;
	gpio_input.GPIO_Pin = GPIO_Pin_4;
	gpio_input.GPIO_PuPd =GPIO_PuPd_NOPULL;
	gpio_input.GPIO_Speed =GPIO_Speed_100MHz;
	gpio_input.GPIO_OType= GPIO_OType_PP;
	GPIO_Init(GPIOA, &gpio_input);
	GPIO_Init(GPIOD, &GPIOInitStruct);

}
void EXTI4_IRQHandler(void )
{
	if(EXTI->PR & EXTI_PR_PR4)
	{
		if(count<=10)
		{
					count++;
					tim_ch.TIM_OCMode = TIM_OCMode_PWM2;
					tim_ch.TIM_OutputState = TIM_OutputState_Enable;
					tim_ch.TIM_Pulse = (168*count)-1;   //25% duty cycle
					TIM_OC4Init(TIM4,&tim_ch);
		}
		else
			count=0;

		EXTI->PR |= EXTI_PR_PR4;
	}
}
int main(void)
{
	ini();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
		TIM_TimeBaseInitTypeDef tim_struct;
		tim_struct.TIM_ClockDivision=TIM_CKD_DIV1;
		tim_struct.TIM_CounterMode=TIM_CounterMode_Up;
		tim_struct.TIM_Period=1679;
		tim_struct.TIM_Prescaler=0;
		tim_struct.TIM_RepetitionCounter =1;
		TIM_TimeBaseInit(TIM4,&tim_struct);
		TIM_OCInitTypeDef tim_ch;
		tim_ch.TIM_OCMode = TIM_OCMode_PWM2;
		tim_ch.TIM_OutputState = TIM_OutputState_Enable;
		tim_ch.TIM_Pulse = 0;   //25% duty cycle
		TIM_OC4Init(TIM4,&tim_ch);
		TIM_Cmd(TIM4,ENABLE);

	EXTI_InitTypeDef exti;
	NVIC_InitTypeDef nvic_struct;
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource9);
	exti.EXTI_Line= EXTI_Line4;
	exti.EXTI_LineCmd =ENABLE;
	exti.EXTI_Mode= EXTI_Mode_Interrupt
	exti.EXTI_Trigger= EXTI_Trigger_Rising_Falling;
	EXTI_Init(&exti);
	nvic_struct.NVIC_IRQChannel=EXTI4_IRQn ;
	nvic_struct.NVIC_IRQChannelCmd= ENABLE;
	nvic_struct.NVIC_IRQChannelPreemptionPriority= 0x00;
	nvic_struct.NVIC_IRQChannelSubPriority= 0x00;
	NVIC_Init(&nvic_struct);
	NVIC_EnableIRQ(EXTI4_IRQn);
    while(1)
    {
    }
}
