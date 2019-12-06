#include "stm32l0xx.h"
#include "_platform.h"


uint32_t i=0;
uint16_t adc_value=0;



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



void gpio_config(void)
{
	RCC->IOPENR |=RCC_IOPENR_IOPAEN;						//Gpio port A clock enable

	GPIOA->MODER  |=GPIO_MODER_MODE4;						//Set gpio_pin_a4 analog mode

	GPIOA->OSPEEDR |=GPIO_OSPEEDER_OSPEED4;					//Set gpio_pin_a4 high speed


}


void adc_config(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;                                //Adc1 clock enable

	ADC1->CR |= ADC_CR_ADEN;							           	  //Adc1 enable

	ADC->CCR |= ADC_CCR_VREFEN;									      //Vref enable

	ADC1->CFGR1 |= ADC_CFGR1_WAIT ; 								  //Wait conversion mode

	ADC1->CFGR1 |= ADC_CFGR1_CONT;									  //select continuous conversion mode 

	ADC1->CFGR1 |= ADC_CFGR1_SCANDIR;                                 //Scan sequence direction (optional)-check it


	ADC1->CHSELR |= ADC_CHSELR_CHSEL4;                                //channel 4 selected

    ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;   //160.5 clock cycle sampling time	

}

void calibrate_config(void)
{


  if ((ADC1->CR & ADC_CR_ADEN) != 0)                                 //Check adc enable?
{
    ADC1->CR &= ~ADC_CR_ADEN;   									   //Set adc disable
  }
  ADC1->CR |= ADC_CR_ADCAL;                                            //Adc calibrate     
                                
  while ((ADC1->ISR & ADC_ISR_EOCAL) == 0);                            //Wait until end of calibration
 
  ADC1->ISR |= ADC_ISR_EOCAL;                                          //Clean EOC flag


}


void adc_enable(void)
{

  ADC1->CR |= ADC_CR_ADEN;                                       // Enable the ADC
  
  while ((ADC1->ISR & ADC_ISR_ADRDY) == 0);                      //Wait until adc is ready
   


}


void adc_disable(void)
{
	if ((ADC1->CR & ADC_CR_ADSTART) != 0)                          //Check adc start status
  {
    ADC1->CR |= ADC_CR_ADSTP; 									   //Set adc status
  }
  while ((ADC1->CR & ADC_CR_ADSTP) != 0) ;                         //Wait until adc stop
  
  ADC1->CR |= ADC_CR_ADDIS;                                        //Adc set disable

  while ((ADC1->CR & ADC_CR_ADEN) != 0);                           //Wait until Adc enable
   


}
/**************************************Calibrate note****************************************************
* Note: The software is allowed to set ADCAL only when the ADC is disabled (ADCAL=0, ADSTART=0,          *
* ADSTP=0, ADDIS=0 and ADEN=0).                                                                           *																										*
*********************************************************************************************************/

int main(void)
{
	gpio_config();
	clock_config();
	adc_config();

	adc_disable();
	calibrate_config();
	adc_enable();
	
	
	
	while(1)
	{

		
        ADC1-> CR |= ADC_CR_ADSTART;                                   //Start ADC conversion.
		
		while((ADC1->ISR & ADC_ISR_EOC) ==0);						   //Wait until end of conversion
		
		adc_value=ADC1->DR;											   //Read value
		

		for(i=0;i<1000000;i++);
	    ADC1->ISR |= ADC_ISR_EOC;									   //clear EOC flag 
		


	}

		
				
	

	
}


