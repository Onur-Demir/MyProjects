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


void timer2_clock_config(void)
{

  
  RCC->APB2ENR |= RCC_APB2ENR_TIM21EN;                          //Enable the peripheral clock of Timer 
  RCC->IOPENR |= RCC_IOPENR_GPIOBEN;                            //Enable the peripheral clock of GPIOB
  GPIOB->MODER &= ~GPIO_MODER_MODE13;   						//Select alternate function mode on GPIOB pin 13
  GPIOB->MODER |= GPIO_MODER_MODE13_1;
  GPIOB->AFR[1] |= 0x600000;           		                	//Select AF6 on PB13 in AFRH for TIM21_CH1

  

  
  TIM21->PSC = 0;                                               //No prescaler
  TIM21->ARR = 5332;                                            //Set value for 3kHZ  pwm signal
  TIM21->CCR1 = 2665;                                           //Duty %50
  TIM21->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 ;			//Select PWM mode 1 on OC1  (OC1M = 110)
  TIM21->CCMR1 |= TIM_CCMR1_OC1PE; 								//Enable preload register on OC1 (OC1PE = 1)
  TIM21->CCER |= TIM_CCER_CC1E; 								//Enable the output on OC1
  TIM21->CR1 |= TIM_CR1_CEN; 									//Enable counter (CEN = 1)
  TIM21->EGR |= TIM_EGR_UG; 									//Force update generation (UG = 1)
  



}

int main(void)
{
	
	clock_config();
	timer2_clock_config();
	
	while(1)
	{
	}	
				
	

	
}


