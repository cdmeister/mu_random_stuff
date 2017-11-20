/**
  ******************************************************************************
  * @file    main.c
  * @author  Moeiz Riaz
  * @version V1.0.0
  * @date    17-September-2017
  * @brief   Main program body
  ******************************************************************************

*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void USART_Init(USART_TypeDef * USARTx){

  // Disable USART
  //USARTx->CR1 &=~(0x1U << 13);
  USARTx->CR1 &=~USART_CR1_UE;

  // Set data Length to 8 bits
  // 00 = 8 bits  01 = 9 bits
  // 10 = 7 bits
  //USARTx->CR1 &= ~(0x1U << 12);
  USARTx->CR1 &=~USART_CR1_M;

  // Select 1 stop bit( bit 13 abd bit 12)
  // 00 = 1 stop bits   01 = 0.5 stop bit
  // 10 = 2 stop bits   11 = 1.5 stop bits
  //USARTx-> CR2 &= ~(0x3U << 12)
  USARTx->CR2 &= ~USART_CR2_STOP;

  // Set parity control as no parity
  // 0 = no parity,
  // 1 = parity enabled ( then, prgram PS bit to select Even or Odd parity
  USARTx->CR1 &= ~USART_CR1_PCE;

  // Oversampling by 16
  // 0 = oversampling by 16, 1 = oversampling by 8
  USARTx->CR1 &= ~USART_CR1_OVER8;

  // Set Baud Rate to 9600 using APB Frequency
  // Max frequency for USART2 (42 MHz)
  // TODO: create function to calculate the value instead of
  //       hardcoding
  USARTx->BRR = 0x1117;

  // Enable transmission and reception
  USARTx->CR1 |= ( USART_CR1_RE| USART_CR1_TE );

  // Enable USART
  USARTx->CR1 |=USART_CR1_UE;


}

void USART_Write(USART_TypeDef * USARTx, uint8_t * str)  {

  // First check if the Transmission Data register is empty
  while(*str != 0){
    while ( !(USARTx->SR & USART_SR_TXE)){};

    // Write Data to the register which will then get shifted out
    USARTx->DR =(*str & 0xFF);

    // Wait until transmission is complete
    while ( !(USARTx->SR & USART_SR_TC)){};
    str++;
  }


}

void USART_Read(USART_TypeDef * USARTx, uint8_t * buffer, uint32_t nBytes){

  int i;
  for(i =0; i<nBytes;i++){
    while(! (USARTx->SR & USART_SR_RXNE));
    buffer[i] = USARTx->DR & 0xFF;
  }

}


/**
  * @brief   Main program
  * @param  None
  * @retval None
  */

int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
        system_stm32f4xx.c file
     */

  // Enable GPIO clock and configure the Tx pin and the Rx pin as
  // Alternating functions, High Speed, Push-pull, Pull-up

  // GPIO Initialization for USART 2
  // PA2 (Tx) PA3 (Rx)
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  // Set mode of all pins as digital output
  // 00 = digital input       01 = digital output
  // 10 = alternate function  11 = analog (default)
  GPIOA->MODER &= ~(0xF<<4); /* Clear mode bits */
  GPIOA->MODER |= 0xA<<4;/* PA2 and PA3  are on GPIOA */

  // Alternate Function 7 = USART 2
  GPIOA->AFR[0] = (0x77 << 8); //Set alternate function for Pins 2 and 3

  // Set output type of all pins as push-pull
  // 0 = push-pull (default)
  // 1 = open-drain
  GPIOA->OTYPER &= ~(0x3<<2); /*Configure as output open-drain */

  // Set output speed of all pins as high
  // 00 = Low speed           01 = Medium speed
  // 10 = Fast speed          11 = High speed
  GPIOA->OSPEEDR &=~(0xF<<4); /* Configure as high speed */
  GPIOA->OSPEEDR |= (0xF<<4);

  // Set all pins as no pull-up, no pull-down
  // 00 = no pull-up, no pull-down    01 = pull-up
  // 10 = pull-down,                  11 = reserved
  GPIOA->PUPDR &= ~(0xF<<4); /*no pul-up, no pull-down*/
  GPIOA->PUPDR |= (0x5<<4); /*no pul-up, no pull-down*/

  // Enable USART2
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

  // Initial USART2
  USART_Init(USART2);


  int i =1;
  while(i>0){
    USART_Write(USART2,(uint8_t *) "Hello World\n\r");
    i--;
  }

  uint8_t buffer[11];

  USART_Read(USART2,buffer,11);
  USART_Write(USART2,(uint8_t *) "Terminal Wrote: ");
  USART_Write(USART2,buffer);
  USART_Write(USART2,(uint8_t*)"\n\r");


  while(1);

  return 0;
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
