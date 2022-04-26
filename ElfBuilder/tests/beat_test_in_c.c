#include "plugin_test.h"
uint32_t counter = 0;

void on_beat(uint32_t frameindex) {
    play_note(frameindex, 0, 64); // turn note off
    if(counter >= 10)
        return;
    
    *putint = ++counter;
    puts(": ");
    *putint = frameindex;
    *putch = '\n';

    play_note(frameindex, 70, 64); // turn note on

    if(counter == 10)
        puts("TEST PASSED\n");
}

void __start() {
    register_event(EVENTID_BEAT, (uint32_t) &on_beat);
    puts("REGISTERED BEAT EVENT\n");
    puts("PRESS PLAY AND LET IT COUNT TO 10.\n");
    puts("IT SHOULD PLAY A NOTE ON EVERY BEAT.\n\n");
}