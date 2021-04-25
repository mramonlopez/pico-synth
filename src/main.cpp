#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "SineWave.h"
#include <midi.h>

// From audio.cpp
void setUpDAC();

stk::SineWave *dco;

int main()
{

    bi_decl(bi_program_description("Pico Synth"));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    stk::Stk::setSampleRate(44100);

    dco = new stk::SineWave();
    dco->setFrequency(440.0f);

    setUpMIDI();
    setUpDAC();

    while (1)
    {
        doMidi(uart_getc(uart0));
        // __wfi(); // Wait for Interrupt
    }
}