#include "stm32f0xx.h"
#include <stdint.h>
#include <stdio.h>

void internal_clock();

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

// set up keyboard for use
void init_keypad()
{
    // enable GPIOC as that is where keyboard is
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // PC0 - PC3 should be set as input pins (00) with the pull down resistor enabled (10)
    // recieving data
    GPIOC->MODER &= ~0xFF;
    GPIOC->PUPDR &= ~0xFF;
    GPIOC->PUPDR |= 0xAA;

    // PC4 - PC7 should be set as output pins (01), for activating coloumns later
    GPIOC->MODER &= ~0xFF00;
    GPIOC->MODER |= 0x5500;

    // should record an input faster than ~100mS, as that is the fastest human reaction time
    // use SysTick as this is similar enough to what Lab 2 was doing, copied and modified

    // (6MHz / LOAD + 1) = frequency; set to 20Hz for full sweep
    // still slow enough that no history is need for anti-bounce
    SysTick->LOAD = 75000 - 1;

    // set to VAL to 0 to ensure that the counter is reset 
    SysTick->VAL = 0;

    SysTick->CTRL &= ~0b100; 
    SysTick->CTRL |= 0b11;
}

// changes col of output for keybaord
void set_col(int col) {
    // set PC 4-7 to be low
    GPIOC->BSRR |= 0xF00000;
  
    // set PC 8 - col to high
    GPIOC->BSRR |= 1 << (8 - col);
}

// same control structure as part 2
volatile int current_col = 1;
// useful for passing information to various sections, though this may require more thought, need history for game needs
volatile char last_char_pressed = '0';

// col then row
const char key_map[4][4] = {{'1', '4', '7', '*'}, 
                            {'2', '5', '8', '0'},
                            {'3', '6', '9', '#'},
                            {'A', 'B', 'C', 'D'}};

// copied over code from lab 2
void SysTick_Handler() 
{
    // first read IDR for value of powered col
    int rows = GPIOC->IDR & 0xF;
    int pressed_row = 0;
    for (int i = 1; i < 5; i++)
    {
        if (rows == 1)
        {
            pressed_row = i;
            break;
        }
        rows = rows >> 1;
    }

    if (pressed_row != 0)
    {
        last_char_pressed = key_map[current_col - 1][4 - pressed_row];
        printf("(%d, %d): %c\n", current_col-1, pressed_row - 1, last_char_pressed);
        // an idea could be to invoke some sort of function response
    }

    // every invocation of SysTick_Handler changes the col that is powered
    current_col = (current_col % 4) + 1;
    // turn on PC 8 - current col while turning off the rest of PC 4-7
    set_col(current_col);
}

int main()
{
    internal_clock();
    init_keypad();
    init_computer_feedback();
    
    // get rid of buffers for printing
    setbuf(stdin,0); 
    setbuf(stdout,0);
    setbuf(stderr,0);

    // let interrupts run the rest of the program
    for(;;) 
    {
        asm("wfi");
    }
}

// planning for matrix implementation:
// https://cdn-learn.adafruit.com/downloads/pdf/32x16-32x32-rgb-led-matrix.pdf
// going to need to convert code from Arduino to C and recreate the package... 
// also need to add wiring for SPI's with registers and connect that 
// consideration: needs exactly 5V (anymore and stuffs broke, less and may not work properly),
// may be get an independent source of 5V; also has high current draw of 4A, so another reason 
// for supporting that

// 3 ground wires for common config - fine, that can all be a shared bus
// For upper: R1, G1 and B1 serve as RGB (cannot use SPI, read https://learn.adafruit.com/adafruit-protomatter-rgb-matrix-library/adding-a-new-device but some documents earlier)
// to output these, which aren't just high or low, also limited bounds, arduino want's 5V, 
// meaning that an op-amp may needed in feedback configuration

// need seperate power source - https://www.youtube.com/watch?v=g7Zrnr4kGw8
// find different github for code
