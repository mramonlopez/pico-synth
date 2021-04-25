#ifndef MIDI_H
#define MIDI_H

#include "pico/stdlib.h"

const uint LED_PIN = 25;
const char MIDICH = 1;

void setUpMIDI();
void doMidiNoteOn(char note, char vel);
void doMidiNoteOff(char note, char vel);
void doMidi(char mb);

#endif // MIDI_H