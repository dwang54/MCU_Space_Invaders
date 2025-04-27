#include "defines.h"

// useful for passing information to various sections, though this may require more thought, need history for game needs
volatile char last_char_pressed = '0';

// same control structure as part 2
volatile int current_col = 1;

// col then row
const char key_map[4][4] = {{'1', '4', '7', '*'}, 
                            {'2', '5', '8', '0'},
                            {'3', '6', '9', '#'},
                            {'A', 'B', 'C', 'D'}};

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
        // an idea could be to invoke some sort of function response
    }

    // every invocation of SysTick_Handler changes the col that is powered
    current_col = (current_col % 4) + 1;
    // turn on PC 8 - current col while turning off the rest of PC 4-7
    set_col(current_col);
}