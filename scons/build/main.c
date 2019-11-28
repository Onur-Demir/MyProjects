//include <stddef.h>



#include "_platform.h"

#include "stm32l0xx.h"

uint32_t i=0;

void gpio_config()
{



		RCC->IOPENR |= RCC_IOPENR_IOPAEN;			// GPIOA clock enable
		RCC->IOPENR |= RCC_IOPENR_IOPCEN;			// GPIOC clock enable



		GPIOA->MODER &= ~GPIO_MODER_MODE5;          // clean register for reset state(MOD5_REG=11)
		GPIOA->MODER |= GPIO_MODER_MODE5_0;         //just mode5 first bit set 1, general purpose output mode


		GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEED5_1;  // port_a output speed to high speed(10)
		GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEED5_1;  // port_c output speed to high speed(10)


		GPIOC->MODER &= ~GPIO_MODER_MODE13;	        // clean register for reset state(MOD5_REG=11)
		GPIOC->MODER |= 0x51555555UL;		        // input for just button(pin_c13)




}


void clock_config(){

	/*  enable power clock  (step:1) */
	RCC->APB1ENR |= (RCC_APB1ENR_PWREN);

	/*PWR->CR = (PWR->CR & ~(PWR_CR_VOS)) | PWR_CR_VOS_0; (voltage scaling for low power)*/

	/* Enable HSI divided by 4 in RCC-> CR (step:2) */  // after change divided value for test

	RCC->CR |=(RCC_CR_HSIDIVEN  );               //HSI divider(16Mhz) by 4
	RCC->CR |=(RCC_CR_HSION  );					//16 MHz high-speed internal clock enable


	// Wait for HSI ready flag and HSIDIV flag (HSI16RDY goes low after 6 HSI16 clock cycles.)
	while ((RCC->CR & (RCC_CR_HSIRDY |RCC_CR_HSIDIVF)) != (RCC_CR_HSIRDY |RCC_CR_HSIDIVF)); /* (4) */

	/* Set PLL on HSI, multiply by 8 and divided by 2(step:3) */

	RCC->CFGR |= RCC_CFGR_PLLSRC_HSI ;          //PLL entry clock source
	RCC->CFGR |= RCC_CFGR_PLLMUL8;				//PLLVCO = PLL clock entry x 8
	RCC->CFGR |= RCC_CFGR_PLLDIV2;				//PLL clock output = PLLVCO / 2

	/*Enable the PLL in RCC_CR register */
	RCC->CR |= RCC_CR_PLLON;

	/*  Wait for PLL ready flag */
	 while ((RCC->CR & RCC_CR_PLLRDY)  == 0);

	/*  Select PLL as system clock */
	 RCC->CFGR |= RCC_CFGR_SW_PLL;


	/* Select PLL as system clock */
	 while ((RCC->CFGR & RCC_CFGR_SWS_PLL)  == 0);
/*-------------------Configure the clock source to be used to drive the System clock------*/
	 RCC->CFGR |= RCC_CFGR_HPRE_DIV1;    		/* SYSCLK not divided (AHB prescaler=1)*/

	 RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;          /* HCLK not divided(APB1 prescaler=1)*/

	// RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;			/* HCLK not divided(APB2 prescaler=1)*/

/*-----------------------------------------------------------------------------------------*/

	 RCC->CICR |= RCC_CICR_HSIRDYC;             /* HSI16 ready Interrupt clear*/


}




int main(void)
{

	clock_config();
	gpio_config();



	while(1)
	{

		GPIOA->ODR ^= (1<<5);
		for(i=0;i <=100000;i++);


	}
}


