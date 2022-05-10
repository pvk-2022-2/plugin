#include "plugin_test.h"


const uint32_t BAT_LEN = 3;
const uint32_t SCREEN_LEN = 8;

uint32_t bat_y = 3;
uint32_t bat_x = 7;


void draw_bat() {
    for (int i = 0; i < SCREEN_LEN; i++) {
        move_grid_cursor(bat_x, i);
        if (i >= bat_y && i < bat_y + BAT_LEN) {
            set_grid_tile(0xffffff, '|');
        } else {
            set_grid_tile(0x0, ' ');
        }

    }
}

void on_press(uint32_t index) {
    uint32_t x = index % GRID_WIDTH;
    uint32_t y = index / GRID_WIDTH;

    if(y < (SCREEN_LEN - 1) / 2) {
        if (bat_y > 0) {
            bat_y -= 1;
        }
        
        return;
    } else if (y > (SCREEN_LEN + 1) / 2){
        if (bat_y < SCREEN_LEN) {
            bat_y += 1;
        }
        return;
    }
    

    draw_bat();
}

uint32_t ball_x = 2;
uint32_t ball_y = 2;
int32_t ball_dx = 1;
int32_t ball_dy = 1;

void on_beat(uint32_t frame_index) {
    uint32_t new_x = ball_x + ball_dx;
    uint32_t new_y = ball_y + ball_dy;


    if (new_x == bat_x) {
        if (new_y > bat_y && new_y < bat_y + BAT_LEN) {
            ball_dx = -ball_dx;
            new_x = ball_x;
        }
    }
    if (new_x == 0) {
        ball_dx = 1;
    } 
    if (new_x == SCREEN_LEN - 1) {
        ball_dx = -1;
    }
    if (new_y == 0) {
        ball_dy = 1;
    } 
    if (new_y == SCREEN_LEN - 1) {
        ball_dy = -1;
    }

    move_grid_cursor(ball_x, ball_y);
    set_grid_tile(0, ' ');

    move_grid_cursor(new_x, new_y);
    set_grid_tile(0xff8080, 'o');

    ball_x, ball_y= new_x, new_y;
}

void __start() {
    register_event(EVENTID_GRIDPRESS, (uint32_t) &on_press);
    register_event(EVENTID_BEAT, (uint32_t) &on_beat);

    puts("PONG TIME!!!!\n");
}
