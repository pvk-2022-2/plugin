#include "plugin_test.h"

uint32_t tile_index = 0;
uint32_t color = 0x00e357;

void on_beat(uint32_t frameindex) {
    set_grid_tile(0x0463c9, 'B');
    move_grid_cursor(tile_index % GRID_WIDTH, tile_index / GRID_HEIGHT);
    
    tile_index ++;
    if(tile_index >= GRID_WIDTH * GRID_HEIGHT)
        tile_index = 0;

    set_grid_tile(color, 'N');
}

void on_press(uint32_t index) {
    tile_index = index;
    puts("press index: ");
    *putint = index;

    *putch = '\n';
}

void __start() {
    register_event(EVENTID_BEAT, (uint32_t) &on_beat);
    register_event(EVENTID_GRIDPRESS, (uint32_t) &on_press);
}