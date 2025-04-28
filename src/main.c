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

    setup_adc();
    init_tim2();

    init_sound_effects();
    init_audio_output();
    nano_wait(10000000000);
    play_sfx(ENEMY_DIE_SID);
    printf("SOUND EFFECT 1\n");
    nano_wait(10000000000);
    play_sfx(LASER_SHOOT_SID);
    printf("SOUND EFFECT 2\n");
    nano_wait(10000000000);
    play_sfx(PLAYER_DEATH_SID);
    printf("SOUND EFFECT 3\n");
    nano_wait(10000000000);
    play_sfx(NO_SID);
    printf("SOUND EFFECT 4\n");

    /*
    for (int i = 0; i < 5; i++)
    {
        nano_wait(1000000000);
        printf("%d\n", get_volume());
    }
    */

    // let interrupts run the rest of the program
    for(;;) 
    {
        asm("wfi");
    }
}