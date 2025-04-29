#include "defines.h"

#define MSG_LENGTH 8

volatile uint16_t display[MSG_LENGTH];

extern const char font[];

void init_spi1(void) {
    // Enable SPI2 peripheral in RCC
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Enable GPIOB in RCC
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Set PA5, PA7, and PA15 to alternative function (10)
    GPIOA->MODER &= ~((0x33 << 10) + (0x3 << 30));
    GPIOA->MODER |= (0x22 << 10) + (0x2 << 30);

    // Set up alternative functions for GPIOB pins selected in AFR (alternative function register)
    // [0] is the low register (pins 0-7), [1] is the high register (pins 8-15); 4 bits per register
    // refer to table 15 in STM32F091xBC.pdf for more information
    GPIOA->AFR[0] &= ~(0xF0F << (4 * 5));
    GPIOA->AFR[1] &= ~(0xF << (4 * 7));

    // Disable SPI2 peripheral to be able to set it up
    SPI1->CR1 &= ~SPI_CR1_SPE;

    // Set Baud rate to slowest possible (frequency of CLK / 256)
    SPI1->CR1 |= 0b111 << 3;

    // Need to avoid setting to 0 then OR-ing causes the value
    // to instead to be set to 8-bit data size then OR-ed; this is because 
    // of not used values; configured to 16 bits (this makes OR-ing ok, but
    // still not general method)
    SPI1->CR2 |= SPI_CR2_DS;

    // set SPI2 to master mode
    SPI1->CR1 |= SPI_CR1_MSTR;

    // Set up output
    SPI1->CR2 |= SPI_CR2_SSOE;
    
    // set SPI2 to have NSS pulse management enabled
    // "generate an NSS pulse between two consecutive data when doing continuous transfers" 
    SPI1->CR2 |= SPI_CR2_NSSP;

    // enables DMA transfers on empty transmission buffer 
    SPI1->CR2 |= SPI_CR2_TXDMAEN;

    // Reenable SPI2 peripheral
    SPI1->CR1 |= SPI_CR1_SPE;
}

void spi1_setup_dma(void) {
    // Enable RCC (clock) for DMA1
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    // Channel 5 selected as it contains TIM15
    // Set CMAR (other pair of address) to the address of the msg array 
    DMA1_Channel3->CMAR = (uint32_t) (display);

    // Set CPAR (peripheral address) to the address of the SPI2->DR register
    DMA1_Channel3->CPAR = (uint32_t) (&(SPI1->DR));

    // Set CNDTR to 8. (the amount of LEDs and the number of elements being transfered between the addresses)
    DMA1_Channel3->CNDTR = 8;

    // Set the DIRection for copying from-memory-to-peripheral (read from memory, 1)
    DMA1_Channel3->CCR |= DMA_CCR_DIR;

    // Set the MINC to increment the CMAR for every transfer. (Each LED will have something different on it, enabled = 1)
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

    // Do not enable DMA in the same function!!! 
}

void set_message(char* msg)
{
    uint16_t temp_array[MSG_LENGTH] = {0x0000,0x0100,0x0200,0x0300,0x0400,0x0500,0x0600,0x0700};
    int i = 0;
    for (i = 0; i < MSG_LENGTH; i++)
    {
        if (msg[i] == '\0')
        {
            break;
        }
        else
        {
            display[i] = temp_array[i] | font[(int) msg[i]];
        }
    }
    while (i < MSG_LENGTH)
    {
        display[i] = temp_array[i] | font[(int)' '];
        i++;
    }
}

void spi1_enable_dma(void) {
    // Enable DMA1 (1)
    DMA1_Channel3->CCR |= DMA_CCR_EN;
}

void init_7_segment_display()
{
    set_message(" ");
    init_spi1();
    spi1_setup_dma();
    spi1_enable_dma();
}