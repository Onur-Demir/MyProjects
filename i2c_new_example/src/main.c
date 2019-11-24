#include "stm32l0xx.h"
#include "stm32l053xx.h"
#include <stdlib.h>

uint8_t receive_data_1=0,receive_data_2=0,count;
uint32_t i=0;





void configure_clock(){

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






void configure_gpio(void)
{

	RCC->IOPENR |= RCC_IOPENR_IOPAEN;			// GPIOA clock enable
	RCC->IOPENR |= RCC_IOPENR_IOPBEN;			// GPIOB clock enable
	RCC->IOPENR |= RCC_IOPENR_IOPCEN;			// GPIOC clock enable



	GPIOA->MODER &= ~GPIO_MODER_MODE5;          // clean register for reset state(MOD5_REG=11)
	GPIOA->MODER |= GPIO_MODER_MODE5_0;         //just mode5 first bit set 1, general purpose output mode


	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEED5_1;  // port_a output speed to high speed(10)
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEED5_1;  // port_c output speed to high speed(10)





	GPIOC->MODER &= ~GPIO_MODER_MODE13;	        // clean register for reset state(MOD5_REG=11)
	//GPIOC->MODER |= 0x41555555UL;		        // input for c13-c14

	GPIOC->MODER &= ~GPIO_MODER_MODE12;	        // clean register for reset state(MOD5_REG=11)
	//GPIOC->MODER |= 0x41555555UL;				// input for c13-c14



}



void configure_i2c2(void)                  //for pin_b7(SDA)-pin_b6(SCL)
{
	/*-----------------------------i2c GPIO pin set--------------------------------------------*/
        RCC->IOPENR |= RCC_IOPENR_IOPBEN;			// GPIOB clock enable




		GPIOB->MODER &=~GPIO_MODER_MODE6_0;		//MOD register set alternate function
		GPIOB->MODER |=	GPIO_MODER_MODE6_1;		//MOD register set alternate function
		GPIOB->MODER &=~GPIO_MODER_MODE7_0;		//MOD register set alternate function
		GPIOB->MODER |=	GPIO_MODER_MODE7_1;		//MOD register set alternate function

		GPIOB->OTYPER |=GPIO_OTYPER_OT_6; 			//type register open drain
		GPIOB->OTYPER |=GPIO_OTYPER_OT_7;			//type register open drain

		GPIOB->OSPEEDR &=~GPIO_OSPEEDER_OSPEED6_0;	//output speed low
		GPIOB->OSPEEDR &=~GPIO_OSPEEDER_OSPEED7_0; //output speed low


		GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD6;			//No pull-up, pull-down register
		GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD7;			//No pull-up, pull-down register

		GPIOB->AFR[0] = 0b00010001000000000000000000000000; 	//set alternate function

	/*-----------------------------------------------------------------------------------------*/

		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;         //i2c2 enable
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;			//i2c1 enable

		I2C1->TIMINGR = (uint32_t)0x00300619; 		//timing register value for 16Mhz i2c clock

		I2C1-> CR1 |= I2C_CR1_PE;					//i2c1 peripheral enable
/*----------------------------------------FIRST STEP(write on epprom mem.adr)---------------------------*/


		I2C1->CR2 |= 0xA0;						// slave device addres

		I2C1->CR2 &=~I2C_CR2_RD_WRN ;			// Master requests a write transfer.

		I2C1->CR2 &=~I2C_CR2_ADD10;			    // set adressing 7 bits

		I2C1->CR2 |= 0x30000;					// set send byte value=1

		I2C1->CR2 |=I2C_CR2_START;				//start generation



		while(!(I2C1->ISR & I2C_ISR_BUSY));		//control i2c busy?
		while(!(I2C1->ISR & I2C_ISR_TXE ));		//control TXD empty?


		I2C1->TXDR = 0x00;						//send byte eeprom memory high address

		while(!(I2C1->ISR & I2C_ISR_BUSY));		//control i2c busy?
		while(!(I2C1->ISR & I2C_ISR_TXE ));		//control TXD empty?

		I2C1->TXDR = 0x64;						//send byte eeprom memory low address


		while(!(I2C1->ISR & I2C_ISR_BUSY));		//control i2c busy?
		while(!(I2C1->ISR & I2C_ISR_TXE ));		//control TXD empty?


		I2C1->TXDR = 0x37;						//send '7' caracter




		while(!(I2C1->ISR & I2C_ISR_TXE ));		//control TXD empty
		while(!(I2C1->ISR & I2C_ISR_BUSY));
		while(!(I2C1->ISR & I2C_ISR_TC) );		//wait for NBYTES data have been transferred






/*----------------------------------SECOND STEP(read eeprom mem. addr.)---------------------*/
		I2C1->CR2 |= 0xA0;						// slave device addres

		I2C1->CR2 &=~I2C_CR2_RD_WRN ;			// Master requests a write transfer.

		I2C1->CR2 &=~I2C_CR2_ADD10;			    // set adressing 7 bits

		I2C1->CR2 &=~0x30000;
		I2C1->CR2 |= 0x20000;					// set send byte value=2

		I2C1->CR2 |=I2C_CR2_START;				//start generation

	/*--------------------------------------------------------------------------------------*/

		while(!(I2C1->ISR & I2C_ISR_BUSY));		//control i2c busy?

		while(!(I2C1->ISR & I2C_ISR_TXE ));		//control TXD empty?

		I2C1->TXDR = 0x00;						//send byte eeprom memory high address



		//while(!(I2C1->ISR)& I2C_)             //read ack status

		while(!(I2C1->ISR & I2C_ISR_BUSY));		//control i2c busy?

		while(!(I2C1->ISR & I2C_ISR_TXE ));		//control TXD empty?


		I2C1->TXDR = 0x64;						//send byte eeprom memory low address


		while(!(I2C1->ISR & I2C_ISR_BUSY));		//control i2c busy?

		while(!(I2C1->ISR & I2C_ISR_TXE ));		//control TXD empty?


	/*--------------------------ReStart for read eeprom mem. addr---------------------------*/

		I2C1->CR2 |= 0xA0;						// slave device addres

		I2C1->CR2 |= I2C_CR2_RD_WRN ;			// Master requests a read transfer.

		I2C1->CR2 &=~I2C_CR2_ADD10;				// set adressing 7 bits





		I2C1->CR2 &=~0x20000;					//clear NBYTES bit
		I2C1->CR2 |= 0x10000;					// set to read value 1 byte


		I2C1->CR2 |=I2C_CR2_START;				//start generation



		//while(!(I2C1->ISR & I2C_ISR_BUSY));		//control i2c busy?

		while(!(I2C1->ISR & I2C_ISR_RXNE));		//rxne empty flag

		receive_data_1 =atoi (&I2C1 -> RXDR);

		I2C1->CR2 |= I2C_CR2_STOP;				//i2c1 stop

		while(I2C1->CR2 & I2C_CR2_STOP);        //wait for start flag


	/*--------------------------------------------------------------------------------------*/



}


void eeprom_read_adr(void);






int main(void)
{

	configure_i2c2();                  //for pin_b7(SDA)-pin_b6(SCL)
	configure_gpio();
	configure_clock();
	eeprom_read_adr();


  while (1)
  {


	  if(!(GPIOC->IDR & GPIO_IDR_ID13))                 //button read pin_C13
	  	  	  {
	  	  		while(!(GPIOC->IDR & GPIO_IDR_ID13));


	  	  		if(receive_data_1 == 7)
	  	  			{
	  	  				GPIOA->ODR |= GPIO_ODR_OD5;

	  	  			}



	  	  	  }







	  if(!(GPIOC->IDR & GPIO_IDR_ID12))            //button read eeprom bank 101
	  	  	  	  {
	  	  	  		while(!(GPIOC->IDR & GPIO_IDR_ID12));


	  	  	  		eeprom_read_adr();		       //read eeprom mem.reg.(0x65) value code


 	  	  	  		if(receive_data_2 == 6)

	  	  	  		   {

	  	  	  		   	  GPIOA->ODR |= GPIO_ODR_OD5;

	  	  	  		   }

	  	  	  		else
	  	  	  		{

	  	  	  			while(1)						//error bank value
	  	  	  			{
	  	  	  			 GPIOA->ODR ^= (1<<5);
	  	  	  			 for(i=0;i <=100000;i++);
	  	  	  			}

	  	  	  		}


	  	  	  	  }






  }


}



void eeprom_read_adr(void)
{
	/*----------------------------------SECOND STEP(read eeprom mem. addr.)---------------------*/
			I2C1->CR2 |= 0xA0;						// slave device addres

			I2C1->CR2 &=~I2C_CR2_RD_WRN ;			// Master requests a write transfer.

			I2C1->CR2 &=~I2C_CR2_ADD10;			    // set adressing 7 bits

			I2C1->CR2 &=~0x30000;
			I2C1->CR2 |= 0x20000;					// set send byte value=2

			I2C1->CR2 |=I2C_CR2_START;				//start generation

		/*--------------------------------------------------------------------------------------*/

			while(!(I2C1->ISR & I2C_ISR_BUSY));		//control i2c busy?

			while(!(I2C1->ISR & I2C_ISR_TXE ));		//control TXD empty?

			I2C1->TXDR = 0x00;						//send byte eeprom memory high address



			//while(!(I2C1->ISR)& I2C_)             //read ack status

			while(!(I2C1->ISR & I2C_ISR_BUSY));		//control i2c busy?

			while(!(I2C1->ISR & I2C_ISR_TXE ));		//control TXD empty?


			I2C1->TXDR = 0x65;						//send byte eeprom memory low address


			while(!(I2C1->ISR & I2C_ISR_BUSY));		//control i2c busy?

			while(!(I2C1->ISR & I2C_ISR_TXE ));		//control TXD empty?


		/*--------------------------ReStart for read eeprom mem. addr---------------------------*/

			I2C1->CR2 |= 0xA0;						// slave device addres

			I2C1->CR2 |= I2C_CR2_RD_WRN ;			// Master requests a read transfer.

			I2C1->CR2 &=~I2C_CR2_ADD10;				// set adressing 7 bits





			I2C1->CR2 &=~0x20000;					//clear NBYTES bit
			I2C1->CR2 |= 0x10000;					// set to read value 1 byte


			I2C1->CR2 |=I2C_CR2_START;				//start generation



			//while(!(I2C1->ISR & I2C_ISR_BUSY));		//control i2c busy?

			while(!(I2C1->ISR & I2C_ISR_RXNE));		//rxne empty flag

			receive_data_2 =atoi (&I2C1 -> RXDR);

			I2C1->CR2 |= I2C_CR2_STOP;				//i2c1 stop

			while(I2C1->CR2 & I2C_CR2_STOP);        //wait for start flag


		/*--------------------------------------------------------------------------------------*/


}
















