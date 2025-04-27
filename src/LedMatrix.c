/*
Peripheral: GPIO and DMA
Usage: Display graphics from graphics structs onto LEd matrix display
Person: Caleb
Needed by any files: Yes; game loop will call functions to render from here on every frame
*/

#include "defines.h"

volatile u_int16_t commands[NUM_ELEMENTS];

// turn off OE for entire col
// color code (on clk), color code (off clk), LAT on, LAT off
// at end of column, set ABCD, then turn on OE

void init_display()
{
    // enable GPIOB in RCC
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // set pins 0 through 12 to all be output (01)
    // most of these pins will need to be manually controlled in update loop
    GPIOB->MODER &= ~0x3FFFFFF;
    GPIOB->MODER |= 0x1555555;

    // set critical values to deactivate OE, CLK, and LAT (B7, B6, and B8 respectively)
    // set OE and LAT to high and CLK to low
    GPIOB->BSRR |= (0x3 << 7) + (0x1 << 22);

    // may not be totally necessary
    // set up alternative function mode for B14 (10)
    GPIOB->MODER &= ~(0x3 << 28);
    GPIOB->MODER |= 0x2 << 28;
    // select in the high alternative function register for B14: TIM15_CH1 (AF1) (0001)
    GPIOB->AFR[1] &= ~(0xF << 24);
    GPIOB->AFR[1] |= 0x1 << 24;

    // code modified from Lab 4
    // Enable RCC (clock) for DMA1
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    // Channel 5 selected as it contains TIM15
    // Set CMAR (other pair of address) to the address of the msg array 
    DMA1_Channel5->CMAR = (uint32_t) (commands);
    // Set CPAR (peripheral address) to the address of the GPIOB_ODR register
    DMA1_Channel5->CPAR = (uint32_t) (&(GPIOB->ODR));
    // Set CNDTR to NUM_ELEMENTS. (the number of elements being transfered between the addresses)
    DMA1_Channel5->CNDTR = NUM_ELEMENTS;
    // Set the DIRection for copying from-memory-to-peripheral (read from memory, 1)
    DMA1_Channel5->CCR |= DMA_CCR_DIR;
    // Set the MINC to increment the CMAR for every transfer.
    DMA1_Channel5->CCR |= DMA_CCR_MINC;
    // Set the MSIZE (memory size) to 16-bit (01)
    DMA1_Channel5->CCR &= ~(0b11 << 10);
    DMA1_Channel5->CCR |= 0b01 << 10;
    // Set PSIZE (peripheral size) to 16-bit (01)
    DMA1_Channel5->CCR &= ~(0b11 << 8);
    DMA1_Channel5->CCR |= 0b01 << 8;
    // Set CIRC to 1 for circular operation
    DMA1_Channel5->CCR |= DMA_CCR_CIRC;

    // enable TIM15 in RCC
    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
    // now set up TIM15
    // set to 48MHz / (PSC + 1) / (ARR + 1)
    TIM15->PSC = 30 - 1; // target 240, allows for 5000nS between signals
    TIM15->ARR = 8 - 1;
    
    // enable DMA requests for TIM15
    TIM15->DIER |= TIM_DIER_UDE;
    // enable CEN bit in TIM15's CR1 (control register 1) to enable timer 15
    TIM15->CR1 |= TIM_CR1_CEN;

    // lastly enable DMA
    DMA1_Channel5->CCR |= DMA_CCR_EN;
}

u_int16_t determine_position_code(int cur_col)
{
    u_int16_t ret_val = 0;
    if (cur_col >= 8)
    {
        ret_val |= D_MASK;
    }
    if ((cur_col >= 4 && cur_col <= 7) ||(cur_col >= 12)) 
    {
        ret_val |= C_MASK;
    }
    if (cur_col == 2 || cur_col == 3 || cur_col == 6 || cur_col == 7 || 
        cur_col == 10 || cur_col == 11 || cur_col == 14 || cur_col == 15)
    {
        ret_val |= B_MASK;
    }
    if (cur_col % 2 == 1)
    {
        ret_val |= A_MASK;
    }
    return ret_val;
}

void init_matrix()
{
    int cur_col = 0;
    for (int index = 0; index < NUM_ELEMENTS; index++)
    {
        commands[index] = OE_MASK; // off by default
        if (index % ARRAY_DEPTH == 0) // if start of new col
        {
            // update cur_col
            cur_col = (cur_col + 1) % MATRIX_LENGTH;
        }
        else if (index % ARRAY_DEPTH == ARRAY_DEPTH - 2) // second to last command in col
        {
            // set ABCD
            commands[index] |= determine_position_code(cur_col);
        }
        else if (index % ARRAY_DEPTH == ARRAY_DEPTH - 1) // last command in col
        {
            // maintain ABCD
            commands[index] |= determine_position_code(cur_col);
            // enable output
            commands[index] &= ~OE_MASK; 
        }
        else 
        {
            // the -1 is to exclude the first command to disable OE
            switch (((index % ARRAY_DEPTH) - 1) % SEGMENT_LENGTH_PER_PIXEL)
            {
                case 0:
                    commands[index] |= CLK_MASK;
                    break;
                case 1:
                    // do nothing, as the CLK is already low
                    break;
                case 2:
                    commands[index] |= LAT_MASK;
                    break;
                case 3: 
                    // do nothing, brings down LAT
                    break;
            }
        }
    }
}

void draw_pixel(int red, int green, int blue, int length, int depth)
{
    // check for out of bounds
    if (depth >= LED_DEPTH || depth < 0 || length >= LED_LENGTH || length < 0)
    {
        return;
    }
    
    // need to invert depth, as the depth in array is reversed
    depth = LED_DEPTH - depth - 1;

    int target_index = -1;
    if (length < 16)
    {
        // the method below leeds to an overshoot of 1 when using length
        length--;
        length = length < 0 ? 15 : length; // if length is negative, overflow to 15
        target_index = ARRAY_DEPTH * length + 1 + depth * SEGMENT_LENGTH_PER_PIXEL; 
        for (int i = 0; i < SEGMENT_LENGTH_PER_PIXEL; i++)
        {
            commands[target_index + i] &= ~(R1_MASK + B1_MASK + G1_MASK);
            commands[target_index + i] |= (red != 0 ? R1_MASK : 0) | (blue != 0 ? B1_MASK : 0) | (green != 0 ? G1_MASK : 0);
        }
    }
    else
    {
        // the method below leeds to an overshoot of 1 when using length
        length -= 17;
        length = length < 0 ? 15 : length; // if length is negative, overflow to 15
        target_index = ARRAY_DEPTH * length + 1 + depth * SEGMENT_LENGTH_PER_PIXEL; 
        for (int i = 0; i < SEGMENT_LENGTH_PER_PIXEL; i++)
        {
            commands[target_index] &= ~(R2_MASK + B2_MASK + G2_MASK);
            commands[target_index] |= (red != 0 ? R2_MASK : 0) | (blue != 0 ? B2_MASK : 0) | (green != 0 ? G2_MASK : 0);
        }
    }
}

void draw_rectangle(int red, int green, int blue, int length, int depth, int dim_length, int dim_depth)
{
    for (int x = 0; x < dim_length; x++)
    {
        for (int y = 0; y < dim_depth; y++)
        {
            draw_pixel(red, green, blue, length + x, depth + y);
        }
    }
}
