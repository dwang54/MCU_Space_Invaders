/*
Peripheral: SPI
Usage: Read .WAV files to render in graphics and convert them to graphics struct for usage in game loop
Person: Taviish
Needed by any files: Yes; will call load functions at beginning of game loop; for both graphics and noise (I Think)
*/

// you can add other functions, but make sure to at least implement the functions provided
// you can change interface if you want though, but something along the lines as already provided might be best, but who knows

#include "defines.h"

#include "fifo.h"
#include "tty.h"
#include "commands.h"
#include <stdio.h>

graphic enemy_graphic = (graphic) {
    .graphic_array = {
        { 0x02FF0000, 0x02FF0000, 0x02FF00FF, 0x02FF0000 },
        { 0x02FF0000, 0x02FF0000, 0x02FF0000, 0x02FF0000 },
        { 0x02FF0000, 0x02FF0000, 0x02FF0000, 0x02FF0000 },
        { 0x02FF0000, 0x02FF0000, 0x02FF0000, 0x02FF0000 }
    },
    .w = 4,
    .h = 4,
    .z_level = 1,
};

graphic player_graphic = (graphic) {
    .graphic_array = {
        { 0x0300FFFF, 0x0300FFFF, 0x0300FF00, 0x0300FF00 },
        { 0x0300FF00, 0x0300FFFF, 0x0300FF00, 0x0300FF00 },
        { 0x0300FFFF, 0x0300FFFF, 0x0300FF00, 0x0300FF00 },
        { 0x0300FFFF, 0x0300FF00, 0x0300FF00, 0x0300FF00 }
    },
    .w = 4,
    .h = 4,
    .z_level = 3,
};

graphic laser_graphic = (graphic) {
    .graphic_array = {
        { 0, 0x01FF00FF, 0x01FF00FF },
        { 0, 0x01FF00FF, 0x01FF00FF }
    },
    .w = 3,
    .h = 2,
    .z_level = 2
};

graphic* load_graphic(G_ID g_id) {
    // can use in load_all_graphics
    switch (g_id) {
        case ENEMY_GID:
            return &enemy_graphic;
        case PLAYER_GID:
            return &player_graphic;
        case LASER_GID:
            return &laser_graphic;
        default:
            return NULL;
    }
}

void init_usart5() {
    // TODO
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART5EN;
  
    //GPIOC TX config
    GPIOC->MODER &= ~(0b11 << GPIO_MODER_MODER12_Pos);
    GPIOC->MODER |= (0b10 << GPIO_MODER_MODER12_Pos);
    GPIOC->AFR[1] |= 0xF<<(4 * 4);
    GPIOC->AFR[1] &= 0x2<<(4 * 4);
  
    //GPIOD RX config
    GPIOD->MODER &= ~(0b11 << GPIO_MODER_MODER2_Pos);
    GPIOD->MODER |= (0b10 << GPIO_MODER_MODER2_Pos);
    // GPIOD->AFR[0] |= 0xF<<(4 * 2);
    // GPIOD->AFR[0] &= 0x2<<(4 * 2);
    GPIOD->AFR[0] &= ~(0xf << (2<<2));
    GPIOD -> AFR[0] |= 2 << (2<<2);
  
  
    USART5->CR1 &= ~(USART_CR1_UE); //disable
    USART5->CR1 &= ~(0b1 << 28 | 0b1 << 12);    //setting word length
    USART5->CR2 &= ~(0b11 << 12);   //Stop bit
    USART5->CR2 &= ~(0b1 << 10); //parity control
    USART5->CR2 &= ~(0b1 << 15); //oversampling by 16
    //USART5->BRR = (48000000/115200);
    USART5-> BRR = 0x1A1;
    USART5->CR1 |= (0b11 << 2);
  
    USART5->CR1 |= (USART_CR1_UE);
  
    while (!(USART5->ISR & (USART_ISR_TEACK | USART_ISR_REACK)));
  }
  
  #define FIFOSIZE 16
  char serfifo[FIFOSIZE];
  int seroffset = 0;
  
  void enable_tty_interrupt(void) {
      USART5->CR1 |= (0b1 << 5);
      USART5->CR3 |= (0b1 << 6);
      NVIC->ISER[0] = (1 << USART3_8_IRQn);
      RCC->AHBENR |= RCC_AHBENR_DMA2EN;
      DMA2->CSELR |= DMA2_CSELR_CH2_USART5_RX;
      DMA2_Channel2->CCR &= ~DMA_CCR_EN;  // First make sure DMA is turned off
      DMA2_Channel2->CMAR = (uint32_t)serfifo;
      DMA2_Channel2->CPAR = (uint32_t)&(USART5->RDR);
      DMA2_Channel2->CNDTR = FIFOSIZE;
      DMA2_Channel2->CCR &= 
          ~(DMA_CCR_DIR |
          DMA_CCR_MSIZE_1 |
          DMA_CCR_MSIZE_0 |
          DMA_CCR_PSIZE_1 |
          DMA_CCR_PSIZE_0 |
          DMA_CCR_MEM2MEM);
      DMA2_Channel2->CCR |=
          DMA_CCR_MINC |
          DMA_CCR_CIRC |
          DMA_CCR_PL_0 |
          DMA_CCR_PL_1 |
          DMA_CCR_TCIE;
  
      DMA2_Channel2->CCR |= DMA_CCR_EN;
  }
  
  void USART3_8_IRQHandler(void) {
      while(DMA2_Channel2->CNDTR != sizeof serfifo - seroffset) {
          if (!fifo_full(&input_fifo))
              insert_echo_char(serfifo[seroffset]);
          seroffset = (seroffset + 1) % sizeof serfifo;
      }
  }
  
  // Works like line_buffer_getchar(), but does not check or clear ORE nor wait on new characters in USART
  char interrupt_getchar() {
      // Wait for a newline to complete the buffer.
      while(fifo_newline(&input_fifo) == 0) {
          asm volatile ("wfi"); // wait for an interrupt
      }
      // Return a character from the line buffer.
      char ch = fifo_remove(&input_fifo);
      return ch;
  }
  
  int __io_getchar(void) {
      return interrupt_getchar();
  }
  
  void init_spi1_slow() {
      RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
      RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  
      GPIOB->MODER &= ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER4 | GPIO_MODER_MODER5);
      GPIOB->MODER |= (GPIO_MODER_MODER3_1 | GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1); //PB3, PB4, PB5 as alternate function
      GPIOB->AFR[0] &= 0xFF000FFF; //PB3, PB4, PB5 as AF0
  
      SPI1->CR1 &= ~(SPI_CR1_SPE);
  
      SPI1->CR1 |= (0b111 << 3); //Baud rate
      SPI1->CR2 |= (0b1111 << 8); //DS
      SPI1->CR2 &= (0b0111 << 8); //DS
      SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI; //SSM and ISS and FIFO threshold
      SPI1->CR2 |= (0b1 << 12);
      SPI1->CR1 |= SPI_CR1_MSTR; //Master mode
  
      SPI1->CR1 |= (SPI_CR1_SPE);
  }
  
  void enable_sdcard() {
      GPIOB->ODR &= ~(0b1 << 13); //PB13
  }
  
  void disable_sdcard() {
      GPIOB->ODR |= (0b1 << 13); //PB13 high
  }
  
  void init_sdcard_io() {
      init_spi1_slow();
      GPIOB->MODER &= ~(0b11 << GPIO_MODER_MODER13_Pos);
      GPIOB->MODER |= (0b01 << GPIO_MODER_MODER13_Pos); //PB2 as output
      disable_sdcard();
  }
  
  void sdcard_io_high_speed() {
      SPI1->CR1 &= ~(SPI_CR1_SPE);
      SPI1->CR1 &= ~SPI_CR1_BR;
      SPI1->CR1 |= SPI_CR1_BR_0; //Baud rate 12MHz
      SPI1->CR1 |= SPI_CR1_SPE;
  }