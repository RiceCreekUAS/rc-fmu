#pragma once

#include <Arduino.h>

// max number of pwm output channels
const int PWM_CHANNELS = 8;

class pwm_t {
private:
    uint16_t gen_pwm_test_value();
    
public:
    uint16_t output_pwm[PWM_CHANNELS];
    void act_gain_defaults();
    void setup(int board);
    void update(uint8_t test_pwm_channel = -1);
    void norm2pwm( float *norm );
};

// a global instance is available for use
extern pwm_t pwm;
