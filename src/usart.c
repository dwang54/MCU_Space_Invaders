#include "defines.h"

// system for testing components, allowing for printing back to computer, copied from Lab 7
void init_computer_feedback() {
    // enable RCC clocks for GPIOC and GPIOD
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->AHBENR |= RCC_AHBENR_GPIODEN;
    
    // Configure PC12 to be routed to USART5_TX
    // Set PC12 in GPIOC to alternative function mode (10)
    GPIOC->MODER &= ~(0x3 << 24);
    GPIOC->MODER |= 0x2 << 24;

    // In the high register, set PC12 to AF2 (USART5_TX) 
    GPIOC->AFR[1] &= ~(0xF << (4 * 4));
    GPIOC->AFR[1] |= 0x2 << (4 * 4);

    // Configure PD2 to be routed to USART5_RX
    // Set PD2 in GPIOD to alternative function mode (10)
    GPIOD->MODER &= ~(0x3 << 4);
    GPIOD->MODER |= 0x2 << 4;
    
    // In the low register, set PD2 to AF2 (USART5_RX)
    GPIOD->AFR[0] &= ~(0xF << (2 * 4));
    GPIOD->AFR[0] |= 0x2 << (2 * 4);

    // enable RCC clock for USART5
    RCC->APB1ENR |= RCC_APB1ENR_USART5EN;

    // disable USART clock (set UE bit in CR1 to 0)
    USART5->CR1 &= ~USART_CR1_UE;

    // Set word size of 8 (change bit M0 and M1 
    // which are not adjacent, 00)
    // could be USART_CR1_M, but felt this was better to illustrate above   
    USART5->CR1 &= ~(USART_CR1_M0 + USART_CR1_M1);  

    // Set number of stop bits to 1 in bits 12 to 13 of CR2 (00)
    USART5->CR2 &= ~(0x3 << 12);

    // Disable parity bit (set bit 10 in CR1 to 0)
    USART5->CR1 &= ~USART_CR1_PCE;
    
    // Set up 16x oversampling (OVER8 bit in CR1, set to 0)
    USART5->CR1 &= ~USART_CR1_OVER8; 

    // Set up Baud rate for USART5 (obtained by refering to Table 96
    // or dividing clock rate by 115200, the target Baud rate)
    USART5->BRR = 0x1A1; 

    // Enable the transmitter and the receiver by setting the RE and TE bits
    // in control register 1 (1 is enabled for both bits (2 and 3 respectively))
    USART5->CR1 |= (USART_CR1_TE + USART_CR1_RE);

    // Reenable USART5 (set UE bit in CR1 to 1)
    USART5->CR1 |= USART_CR1_UE;

    // wait for both RE and TE to be ready by checking TEACK and REACK in ISR
    while (!(USART5->ISR & USART_ISR_TEACK) || !(USART5->ISR & USART_ISR_REACK));
}

// necessary override for ensuring that printing works through UART
int __io_putchar(int c) {
    // if character is newline, instead first \r
    if (c == '\n')
    {
        while(!(USART5->ISR & USART_ISR_TXE));
        USART5->TDR = '\r';
    }

    while(!(USART5->ISR & USART_ISR_TXE));
    USART5->TDR = c;
    return c;
}