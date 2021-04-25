#include <midi.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "SineWave.h"

char MIDIRunningStatus;
char MIDINote;
char MIDILevel;

extern stk::SineWave *dco;

float freqs[128] = {8.175799f, 8.661958f, 9.177023f, 9.722718f, 10.300862f, 10.913381f, 11.562326f, 12.249859f, 12.978270f, 13.750000f, 14.567619f, 15.433851f, 16.351598f, 17.323916f, 18.354046f, 19.445436f, 20.601725f, 21.826762f, 23.124651f, 24.499717f, 25.956541f, 27.500000f, 29.135233f, 30.867708f, 32.703196f, 34.647827f, 36.708098f, 38.890873f, 41.203442f, 43.653531f, 46.249303f, 48.999427f, 51.913090f, 55.000000f, 58.270467f, 61.735416f, 65.406391f, 69.295654f, 73.416196f, 77.781746f, 82.406885f, 87.307063f, 92.498606f, 97.998854f, 103.826180f, 110.000000f, 116.540944f, 123.470822f, 130.812783f, 138.591319f, 146.832380f, 155.563492f, 164.813783f, 174.614111f, 184.997211f, 195.997723f, 207.652343f, 220.000000f, 233.081878f, 246.941654f, 261.625565f, 277.182627f, 293.664772f, 311.126984f, 329.627559f, 349.228229f, 369.994423f, 391.995435f, 415.304697f, 440.000000f, 466.163762f, 493.883303f, 523.251131f, 554.365266f, 587.329532f, 622.253967f, 659.255105f, 698.456472f, 739.988845f, 783.990861f, 830.609407f, 880.000000f, 932.327549f, 987.766575f, 1046.502261f, 1108.730554f, 1174.659039f, 1244.507935f, 1318.510264f, 1396.912887f, 1479.977691f, 1567.981787f, 1661.218745f, 1760.000000f, 1864.654943f, 1975.533314f, 2093.004522f, 2217.460926f, 2349.318273f, 2489.015870f, 2637.020310f, 2793.826005f, 2959.955382f, 3135.963315f, 3322.437764f, 3520.000000f, 3729.309887f, 3951.066628f, 4186.009045f, 4434.921851f, 4698.636546f, 4978.031740f, 5274.040620f, 5587.652011f, 5919.910763f, 6271.926630f, 6644.875527f, 7040.000000f, 7458.621006f, 7902.131949f, 8372.018090f, 8869.845168f, 9397.271538f, 9956.063479f, 10548.082983f, 11175.302175f, 11839.821527f, 12543.855333f};

void setUpMIDI()
{
    // Initialise UART 0 to MIDI standard
    uart_init(uart0, 31250);
    //uart_set_format(uart0, 8, 1, UART_PARITY_NONE);

    // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
}

void doMidiNoteOn(char note, char vel)
{
    gpio_put(LED_PIN, 1);
    dco->setFrequency(freqs[note]);
    //print("Note On \t", note, "\t", vel);
}

void doMidiNoteOff(char note, char vel)
{
    gpio_put(LED_PIN, 0);
    dco->setFrequency(0);
    //print("Note Off\t", note, "\t", vel);
}

void doMidi(char mb)
{
    if ((mb >= 0x80) && (mb <= 0xEF))
    {
        // MIDI Voice Category Message.
        // Action: Start handling Running Status
        MIDIRunningStatus = mb;
        MIDINote = 0;
        MIDILevel = 0;
    }
    else if ((mb >= 0xF0) && (mb <= 0xF7))
    {

        // MIDI System Common Category Message.
        // Action: Reset Running Status.
        MIDIRunningStatus = 0;
    }
    else if ((mb >= 0xF8) && (mb <= 0xFF))
    {
        // System Real-Time Message.
        // Action: Ignore these.
        return;
    }
    else
    {
        // MIDI Data
        if (MIDIRunningStatus == 0)
            // No record of what state we're in, so can go no further
            return;

        if (MIDIRunningStatus == (0x80 | (MIDICH - 1)))
        {
            // Note OFF Received
            if (MIDINote == 0)
            {
                // Store the note number
                MIDINote = mb;
            }
            else
            {
                // Already have the note, so store the level
                MIDILevel = mb;
                doMidiNoteOff(MIDINote, MIDILevel);
                MIDINote = 0;
                MIDILevel = 0;
            }
        }
        else if (MIDIRunningStatus == (0x90 | (MIDICH - 1)))
        {
            // Note ON Received
            if (MIDINote == 0)
            {
                // Store the note number
                MIDINote = mb;
            }
            else
            {
                // Already have the note, so store the level
                MIDILevel = mb;
                if (MIDILevel == 0)
                {
                    doMidiNoteOff(MIDINote, MIDILevel);
                }
                else
                {
                    doMidiNoteOn(MIDINote, MIDILevel);
                    MIDINote = 0;
                    MIDILevel = 0;
                }
            }
        }
        else
        {
            // This is a MIDI command we aren't handling right now
            return;
        }
    }
}