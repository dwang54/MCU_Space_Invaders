/*
Peripheral: ADC
Usage: Sample voltages from potentiometer to set volume accordingly
Person: Francis
Needed by any files: No; will be self contained
*/

#include "stm32f0xx.h"

uint32_t volume = 2048;

void setup_adc(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;          // Enable RCC for GPIOA
    GPIOA->MODER |= 0b11 << 2;                  // Pa1 is associated with ADC_IN!; analog mode is 0b11
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;         // Enable RCC For ADC
    RCC->CR2 |= RCC_CR2_HSI14ON;                // Enable 14MHz high-speed internal clock
    while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0) {}   // wait for oscillator to be ready
    ADC1->CR |= ADC_CR_ADEN;                    // enable ADC
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {} // wait for ADC ISR to be readt
    ADC1->CHSELR = 0;                           // disable all channels
    ADC1->CHSELR |= 1 << 1;                     // enable channel select 1 
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {} // wait for adc to be ready
}

#define BCSIZE 32
int bcsum = 0;
int boxcar[BCSIZE];
int bcn = 0;

//============================================================================
// Timer 2 ISR
//============================================================================
// Write the Timer 2 ISR here.  Be sure to give it the right name.

void TIM2_IRQHandler() {
    TIM2->SR &= ~TIM_SR_UIF;         // Acknowledge interrupt
    ADC1->CR |= ADC_CR_ADSTART;      // turn on ADSTART bit
    while ((ADC1->ISR & ADC_ISR_EOC) == 0) {} // wait for end of conversion
    bcsum -= boxcar[bcn];               
    bcsum += boxcar[bcn] = ADC1->DR; // read converted value from ADC_DR
    bcn += 1;
    if (bcn >= BCSIZE) bcn = 0;
    volume = bcsum / BCSIZE; 
}

//============================================================================
// init_tim2()
//============================================================================
void init_tim2(void) {
    RCC->APB1ENR |=  RCC_APB1ENR_TIM2EN;
    // 48 MHZ / 1600 / 3000 == 1 
    TIM2->PSC = 1600 - 1;
    TIM2->ARR = 3000 - 1;
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] = 1 << TIM2_IRQn;
    TIM2->CR1 |= TIM_CR1_CEN;
}

uint32_t get_volume() {
    return volume;
}