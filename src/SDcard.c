/*
Peripheral: SPI
Usage: Read .WAV files to render in graphics and convert them to graphics struct for usage in game loop
Person: Taviish
Needed by any files: Yes; will call load functions at beginning of game loop; for both graphics and noise (I Think)
*/

// you can add other functions, but make sure to at least implement the functions provided
// you can change interface if you want though, but something along the lines as already provided might be best, but who knows

#include "defines.h"

// read from SD cards all the graphics and such
// will return an array of graphics
graphic* load_all_graphics() {
    // load in the graphics


    return NULL;
}

// you could make a load_graphic(G_ID g_id)
// I do not know if it will be used outside this file; will be discussed when game loop gets worked on and others start working on peripheral
graphic* load_graphic(G_ID g_id) {
    // can use in load_all_graphics
}

// sounds can be quite large as each needs a table
// will have to consider if STM has enough memory (probably does)
// if you find the SD card fast enough, you can make this function load_sound(S_ID id), and simply load
// on command, but I doubt it will be fast enough for that

// returns array of sound effects
sfx* load_all_sounds() {
    return NULL;
}

// load in a specific sfx
// this s so we only have on sfx at a time; you can also use this in load_all_sounds (probably better to)
sfx load_sound(S_ID sfx_id) {

}
