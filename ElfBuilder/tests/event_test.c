#include "plugin_test.h"

void note_on(uint32_t note) {
    play_note(0, 70, note);
    play_note(0, 70, note + 2);
    play_note(0, 70, note + 4);

    puts("HIT NOTE ");
    *putint = note;
    *putch = '\n';
}

void note_off(uint32_t note) {
    play_note(0, 0, note);
    play_note(0, 0, note + 2);
    play_note(0, 0, note + 4);

    puts("RELEASED ");
    *putint = note;
    *putch = '\n';
}

void __start() {
    register_event(EVENTID_NOTEON, (uint32_t)&note_on);
    register_event(EVENTID_NOTEOFF, (uint32_t)&note_off);
    puts("REGISTERED EVENTS.\n");
}