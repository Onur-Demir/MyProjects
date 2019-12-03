#include "stm32l0xx.h"
#include "_platform.h"


uint32_t i=0;




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

	 RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;			/* HCLK not divided(APB2 prescaler=1)*/

/*-----------------------------------------------------------------------------------------*/

	 RCC->CICR |= RCC_CICR_HSIRDYC;             /* HSI16 ready Interrupt clear*/

}

void configure_gpio(void)
{	
	RCC->IOPENR |=RCC_IOPENR_IOPAEN;
	RCC->IOPENR |=RCC_IOPENR_IOPBEN;
	RCC->IOPENR |=RCC_IOPENR_IOPCEN;

/**********************config for pin_a5(user_led)**********************************/
	GPIOA->MODER &=~GPIO_MODER_MODE5;			//Set gpio_pin_a5 input mode
	GPIOA->MODER |= GPIO_MODER_MODE5_0;
	GPIOA->OSPEEDR |=GPIO_OSPEEDER_OSPEED5;		//Set gpio_pin_a5 very high speed
	GPIOA->PUPDR  &=~GPIO_PUPDR_PUPD5;			//Set gpio_pin_a5 no pullup-pull down


	/**********************config for pin_a0********************************************/

	GPIOA->MODER &= ~GPIO_MODER_MODE0;          //Set pin_a0 input mode
	GPIOA->OSPEEDR |=GPIO_OSPEEDER_OSPEED0 ;    //Set gpio_pin_a0  very high speed
	GPIOA->PUPDR &=~GPIO_PUPDR_PUPD0; 			//Set gpio_pin_a0 no pullup-pulldown 

	/**********************config for pin_b0********************************************/

	GPIOB->MODER &= ~GPIO_MODER_MODE0;          //Set pin_b0 input mode
    GPIOB->OSPEEDR |=GPIO_OSPEEDER_OSPEED0 ;    //Set gpio_pin_b0  very high speed
	GPIOB->PUPDR &=~GPIO_PUPDR_PUPD0; 			//Set gpio_pin_b0 no pullup-pulldown 
	
	
	



}



void configure_external_it(void)
{
	RCC->APB2ENR |=RCC_APB2ENR_SYSCFGEN;            //SYSCFGN clock enable !!
	RCC->IOPENR |=RCC_IOPENR_IOPAEN;
	RCC->IOPENR |=RCC_IOPENR_IOPBEN;
	RCC->IOPENR |=RCC_IOPENR_IOPCEN;

	

	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB;   //Select Port B for pin 0 extended interrupt(EXTICR) 
	EXTI->IMR |= EXTI_IMR_IM0; 					    //Set bit pin_a0 interrupt not mask
	EXTI->RTSR |=EXTI_RTSR_RT0;                     //Set bit rising trigger enable
	

	NVIC_EnableIRQ(EXTI0_1_IRQn);				    //Enable exti0_1_irqn
	NVIC_SetPriority(EXTI0_1_IRQn,0);			    //Set interrupt priority(0)


}


void EXTI0_1_IRQHandler (void)
{
	if((EXTI->PR & EXTI_PR_PIF0 )!=0)
	{	
		EXTI->PR = EXTI_PR_PR0;                  // Clear the pending bit 

		GPIOA->ODR ^=(1<<5);                     //Toggle user led(pin_a0)

	}

}



int main(void)
{
	
	clock_config();
	configure_gpio();
	configure_external_it();
	
	while(1)
	{
		
	}

		
				
	

	
}


