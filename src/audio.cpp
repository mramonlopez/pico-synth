// Output PWM signals on pins 0 and 1

#include "pico/stdlib.h"
#include "hardware/irq.h"  // interrupts
#include "hardware/pwm.h"  // pwm
#include "hardware/sync.h" // wait for interrupt
#include "SineWave.h"
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"
// Audio PIN is to match some of the design guide shields.
#define AUDIO_PIN 16 // you can change this to whatever you like

extern stk::SineWave *dco;

const uint TOP = 660;
const float DIV_INT = 4;
const float DIV_FRAC = 8;

/*
 * PWM Interrupt Handler which outputs PWM level and advances the 
 * current sample. 
 * 
 * We repeat the same value for 8 cycles this means sample rate etc
 * adjust by factor of 8   (this is what bitshifting <<3 is doing)
 * 
 */
void pwm_interrupt_handler()
{
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));

    // Convert from [-1, 1] to [0, 255]
    auto sample = (int)((dco->tick() + 1.0f) * TOP / 2.0f);
    pwm_set_gpio_level(AUDIO_PIN, sample);
}

void setUpDAC()
{
    //set_sys_clock_khz(176000, true);
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);

    int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);

    // Setup PWM interrupt to fire when PWM cycle is complete
    pwm_clear_irq(audio_pin_slice);
    pwm_set_irq_enabled(audio_pin_slice, true);
    // set the handle function above
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    // Setup PWM for audio output
    pwm_config config = pwm_get_default_config();
    /* Base clock 176,000,000 Hz divide by wrap 250 then the clock divider further divides
     * to set the interrupt rate. 
     * 
     * 11 KHz is fine for speech. Phone lines generally sample at 8 KHz
     * 
     * 
     * So clkdiv should be as follows for given sample rate
     *  8.0f for 11 KHz
     *  4.0f for 22 KHz
     *  2.0f for 44 KHz etc
     */
    pwm_config_set_clkdiv_mode(&config, PWM_DIV_FREE_RUNNING);
    pwm_config_set_clkdiv(&config, (float) clock_get_hz(clk_sys) / (44100.0f * TOP));
    pwm_config_set_wrap(&config, TOP);
    pwm_init(audio_pin_slice, &config, true);

    pwm_set_gpio_level(AUDIO_PIN, 0);
}
