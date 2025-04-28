#include "defines.h"

int main()
{
    internal_clock();
    init_keypad();
    init_computer_feedback();
    
    init_matrix();
    init_display();

    draw_rectangle(1, 1, 1, 16, 32, 8, 8);
    draw_rectangle(0, 0, 1, 2, 4, 10, 10);
    draw_pixel(1, 1, 0, 31, 62);

    nano_wait(1000000000);
    init_7_segment_display();
    set_message("HELLO");

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