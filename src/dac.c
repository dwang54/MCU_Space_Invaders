/*
Peripheral: DAC
Usage: Play sound on speaker
Person: Daniel 
Needed by any files: Yes; game loop to call play_sfx
*/

#include "defines.h"

// This is pretty self contained, so you can do anything you want
// I do not know how this stuff works too well either

// You can attempt to preload the sfx if you want; consider the memory usage
// You can ask for a load_sfx from the SD card file; make sure to include that file

sfx sfx_1, sfx_2; // an example of how to store the sound effects

void play_sfx(S_ID sfx_id) {

}