# Space Invaders on STM32 

This project leverages the STM32’s peripherals to recreate the classic Space Invaders experience. A C game loop drives graphics on the LED matrix via DMA and streams SD‑card‑stored sound effects through the DAC, with real‑time volume adjustment handled by the ADC through a potentionmeter. Meanwhile, SPI updates a seven‑segment display to present welcome messages. 
