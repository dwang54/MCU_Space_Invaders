/*
Peripheral: GPIO 
Usage: Display graphics from graphics structs onto LEd matrix display
Person: Caleb
Needed by any files: Yes; game loop will call functions to render from here on every frame
*/

#include "defines.h"

uint8_t world_graphic[LCD_HEIGHT][LCD_WIDTH]; // will display on each render


// clear the world_graphic
// put all the new entities onto the world to be displayed
void display_entities(graphic* entities) {

}

// just another function in case we do some middle stuff between giving the world all the entities and actually displaying the world
void display_world() {

}