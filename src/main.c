#include "defines.h"

int main()
{
    internal_clock();
    init_keypad();
    init_computer_feedback();
    
    init_matrix();
    init_display();

    draw_rectangle(1, 1, 1, 16, 32, 8, 8);
    
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