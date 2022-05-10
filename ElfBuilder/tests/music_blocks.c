#include "plugin_test.h"

uint32_t start_plane[8][8] = {0};
uint32_t live_plane[8][8] = {0};

uint32_t live_particles[64];
uint32_t particle_count = 0;

uint8_t A[8] = {33, 45, 57, 69, 81, 93, 105};
uint8_t major_scale[8] = { 0, 2, 2, 1, 2, 2, 2, 1 };


uint32_t colors[5] = {0x000000, 0x3d3509, 0x47000d, 0x442703, 0x004732};
char symbols[5] = " <>^v";
uint32_t bonk_color = 0xf9002e;

void draw_tile(uint32_t color, uint32_t x, uint32_t y, char ch) {
    move_grid_cursor(x, y);
    set_grid_tile(color, ch);
}

void on_press(uint32_t index) {
    uint32_t x = index % GRID_WIDTH;
    uint32_t y = index / GRID_WIDTH;

    if(*is_playing)
        return;

    start_plane[x][y]++;
    start_plane[x][y] %= 5;
    draw_tile(colors[start_plane[x][y]], x, y, symbols[start_plane[x][y]]);
}

void on_play() {
    particle_count = 0;
    puts("PLAYING\n");

    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++) {
            if(start_plane[i][j]) {
                live_particles[particle_count++] = (j*8 + i) | (start_plane[i][j] << 16);
                live_plane[i][j] = 1;
            }
        }
}

void on_pause() {
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++) {
            draw_tile(colors[start_plane[i][j]], i, j, symbols[start_plane[i][j]]);
            live_plane[i][j] = 0;
        }

    puts("RESET GRID\n");
}

uint8_t compute_note(const uint32_t x, const uint32_t y) {
    return A[x] + major_scale[y];
}

void on_beat(uint32_t frame_index) {
    if(*is_playing == 0)
        return;

    for(int i = 0; i < particle_count; i++) {
        const uint32_t index = live_particles[i] & 0xFFFF;
        uint32_t x = index % GRID_WIDTH;
        uint32_t y = index / GRID_WIDTH;

        live_plane[x][y] -= 1;
        if(live_plane[x][y])
            play_note(frame_index, 0, compute_note(x, y));
        else
            draw_tile(colors[0], x, y, symbols[0]);

        uint32_t direction = live_particles[i] >> 16;
        uint32_t bonk = 0;
        switch(direction) {
            case 1: if(x == 0) {direction = 2; bonk = 1;} break;
            case 2: if(x >= 7) {direction = 1; bonk = 1;} break;
            case 3: if(y == 0) {direction = 4; bonk = 1;} break;
            case 4: if(y >= 7) {direction = 3; bonk = 1;} break;
        }

        switch(direction) {
            case 1: x -= 1; break;
            case 2: x += 1; break;
            case 3: y -= 1; break;
            case 4: y += 1; break;
        }

        live_plane[x][y] += 1;
        if(live_plane[x][y] > 1) {
            draw_tile(bonk_color, x, y, 'X');
            bonk = 1;
        } 
        else draw_tile(colors[direction], x, y, symbols[direction]);
        
        if(bonk)
            play_note(frame_index, 60, compute_note(x, y));

        live_particles[i] = (x + y*8) | (direction << 16);
    }
}

void __start() {
    register_event(EVENTID_GRIDPRESS, (uint32_t) &on_press);
    register_event(EVENTID_ONPLAY, (uint32_t) &on_play);
    register_event(EVENTID_ONPAUSE, (uint32_t) &on_pause);
    register_event(EVENTID_BEAT, (uint32_t) &on_beat);

    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            draw_tile(colors[0], i, j, symbols[0]);

    puts("Press grid to place note things!!!!\n");
}