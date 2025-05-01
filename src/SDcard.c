/*
Peripheral: SPI
Usage: Read .WAV files to render in graphics and convert them to graphics struct for usage in game loop
Person: Taviish
Needed by any files: Yes; will call load functions at beginning of game loop; for both graphics and noise (I Think)
*/

// you can add other functions, but make sure to at least implement the functions provided
// you can change interface if you want though, but something along the lines as already provided might be best, but who knows

#include "defines.h"

graphic enemy_graphic = (graphic) {
    .graphic_array = {
        { 0x02FF0000, 0x02FF0000, 0x02FF0000, 0x02FF0000 },
        { 0x02FF0000, 0x02FF0000, 0x02FF0000, 0x02FF0000 },
        { 0x02FF0000, 0x02FF0000, 0x02FF0000, 0x02FF0000 },
        { 0x02FF0000, 0x02FF0000, 0x02FF0000, 0x02FF0000 }
    },
    .w = 4,
    .h = 4,
    .z_level = 1,
};

graphic player_graphic = (graphic) {
    .graphic_array = {
        { 0x0300FFFF, 0x0300FFFF, 0x0300FF00, 0x0300FF00 },
        { 0x0300FF00, 0x0300FFFF, 0x0300FF00, 0x0300FF00 },
        { 0x0300FFFF, 0x0300FFFF, 0x0300FF00, 0x0300FF00 },
        { 0x0300FFFF, 0x0300FF00, 0x0300FF00, 0x0300FF00 }
    },
    .w = 4,
    .h = 4,
    .z_level = 3,
};

graphic laser_graphic = (graphic) {
    .graphic_array = {
        { 0, 0x01FF00FF, 0x01FF00FF },
        { 0, 0x01FF00FF, 0x01FF00FF }
    },
    .w = 3,
    .h = 2,
    .z_level = 2
};

graphic* load_graphic(G_ID g_id) {
    // can use in load_all_graphics
    switch (g_id) {
        case ENEMY_GID:
            return &enemy_graphic;
        case PLAYER_GID:
            return &player_graphic;
        case LASER_GID:
            return &laser_graphic;
        default:
            return NULL;
    }
}