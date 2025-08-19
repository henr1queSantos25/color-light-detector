#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"

#define BUZZER_FREQUENCY 100

// Initialize PWM for the buzzer
void pwm_init_buzzer(uint pin);