#include "plugin_test.h"

uint32_t colors[] = {0xff6961, 0xffb480,0xf8f38d,0x42d6a4, 0x08cad1, 0x59adf6, 0x9d94ff, 0xc780e8};
char message[8] = " PAINT! ";

uint32_t selected_color = 0xff6961;

void draw_tile(uint32_t color, uint32_t x, uint32_t y, char ch) {
    move_grid_cursor(x, y);
    set_grid_tile(color, ch);
}

void on_press(uint32_t index) {
    uint32_t x = index % GRID_WIDTH;
    uint32_t y = index / GRID_WIDTH;

    if(y == 0) {
        selected_color = colors[x];
        return;
    }

    draw_tile(selected_color, x, y, ' ');
}

void __start() {
    register_event(EVENTID_GRIDPRESS, (uint32_t) &on_press);

    for(int i = 0; i < 8; i++)
        draw_tile(colors[i], i, 0, message[i]);

    puts("PRESS ANY OF THE COLORED TILES TO DRAW!!!!\n");
}