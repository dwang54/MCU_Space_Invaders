#include "defines.h"

#define MSG_LENGTH 8

volatile uint16_t display[MSG_LENGTH];
extern const char font[];

void init_spi1() {
    // RCC enable for SPI1
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // RCC enable for GPIOA
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Set PA5, PA7, and PA15 to alternative function (10)
    GPIOA->MODER &= ~((0x33 << 10) + (0x3 << 30));
    GPIOA->MODER |= (0x22 << 10) + (0x2 << 30);

    // Set up alternative functions for GPIOA pins selected in AFR (alternative function register)
    // [0] is the low register (pins 0-7), [1] is the high register (pins 8-15); 4 bits per register
    // refer to table 14 in STM32F091xBC.pdf for more information
    GPIOA->AFR[0] &= ~(0xF0F << (4 * 5));
    GPIOA->AFR[1] &= ~(0xF << (4 * 7));

    // Disable SPI1 peripheral to be able to set it up
    SPI1->CR1 &= ~SPI_CR1_SPE;

    // Set Baud rate to slowest possible (frequency of CLK / 256)
    SPI1->CR1 |= 0b111 << 3;

    // Need to avoid setting to 0 then OR-ing causes the value
    // to instead to be set to 8-bit data size then OR-ed; this is because 
    // of not used values; setting value to 16-bit data size
    SPI1->CR2 = 0xF << 8;

    // set SPI1 to master mode
    SPI1->CR1 |= SPI_CR1_MSTR;

    // Set up output
    SPI1->CR2 |= SPI_CR2_SSOE;
    
    // set SPI1 to have NSS pulse management enabled
    // "generate an NSS pulse between two consecutive data when doing continuous transfers" 
    SPI1->CR2 |= SPI_CR2_NSSP;

    // enables DMA transfers on empty transmission buffer 
    SPI1->CR2 |= SPI_CR2_TXDMAEN;    

    // Reenable SPI1 peripheral
    SPI1->CR1 |= SPI_CR1_SPE;
}

void spi1_setup_dma(void) {
    // enable DMA1 in RCC
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    // Choose DMA1 Channel 3 as it contains the TX (transmission) of SPI1
    // Set the memory address to be read from as display
    DMA1_Channel3->CMAR = (uint32_t *) display;

    // Set the peripheral address to SPI1's data register
    DMA1_Channel3->CPAR = (uint32_t *) &(SPI1->DR);
    
    // Set the number of data register to be 16 (16 'packets' to be sent)
    DMA1_Channel3->CNDTR = 16;

    // Set the DIRection for copying from-memory-to-peripheral (read from memory, 1)
    DMA1_Channel3->CCR |= DMA_CCR_DIR;

    // Set the MINC to increment the CMAR for every transfer. (Each LED will have something different, enabled = 1)
    DMA1_Channel3->CCR |= DMA_CCR_MINC;

    // Set the MSIZE (memory size) to 16-bit (01)
    DMA1_Channel3->CCR &= ~(0b11 << 10);
    DMA1_Channel3->CCR |= 0b01 << 10;

    // Set PSIZE (peripheral size) to 16-bit (01)
    DMA1_Channel3->CCR &= ~(0b11 << 8);
    DMA1_Channel3->CCR |= 0b01 << 8;

    // Set CIRC to 1 for circular operation
    DMA1_Channel3->CCR |= DMA_CCR_CIRC;

    // enables DMA transfers on empty transmission buffer, should be fine to have twice 
    SPI1->CR2 |= SPI_CR2_TXDMAEN;

    // enable DMA
    DMA1_Channel3->CCR |= DMA_CCR_EN;
}

void set_message(char* msg)
{
    int i = 0;
    for (i = 0; i < MSG_LENGTH; i++)
    {
        if (msg[i] == '\0')
        {
            break;
        }
        else
        {
            display[i] = font[msg[i]];
        }
    }
    while (i < MSG_LENGTH)
    {
        display[i] = font[' '];
        i++;
    }
    for (i = 0; i < MSG_LENGTH; i++)
    {
        printf("%x\n", display[i]);
    }
    printf("\n");
}

void init_7_segment_display()
{
    set_message(" ");
    init_spi1();
    spi1_setup_dma();
}