#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include <rom/ets_sys.h>
#include <cmath>

extern "C" {

    uint32_t pwm{0};
    bool condition{true};
        
    void _pulse (ledc_timer_config_t *timer_1, ledc_channel_config_t *channel_1);
    void _steady (uint8_t *pwm, ledc_timer_config_t *timer_1, ledc_channel_config_t *channel_1);

    void _pulse (ledc_timer_config_t *timer_1, ledc_channel_config_t *channel_1) {
        if (condition) {
            ledc_set_duty((*channel_1).speed_mode, (*channel_1).channel, pwm++);
            ledc_update_duty((*channel_1).speed_mode, (*channel_1).channel);
            ets_delay_us(500);  //0.5 [ms]
            if (pwm >= (pow(2, (*timer_1).duty_resolution) - 1) * 0.5) condition = !condition;
        }
        else {
            ledc_set_duty((*channel_1).speed_mode, (*channel_1).channel, pwm--);
            ledc_update_duty((*channel_1).speed_mode, (*channel_1).channel);
            ets_delay_us(500);  //0.5 [ms]
            if (pwm <= 0) condition = !condition;
        }
    }

    void _steady (uint8_t *pwm, ledc_timer_config_t *timer_1, ledc_channel_config_t *channel_1) {
        if ((*pwm) <= 0) {
            (*pwm) = 0;
        }
        else if ((*pwm) >= 100) {
            (*pwm) = 100;
        }
        //
        ledc_set_duty((*channel_1).speed_mode, (*channel_1).channel, static_cast<uint32_t>((pow(2, (*timer_1).duty_resolution) - 1) * ((*pwm) / 100.0)));
        ledc_update_duty((*channel_1).speed_mode, (*channel_1).channel);
    }


    void app_main () {
        //timer conf
        ledc_timer_config_t timer_1{};
        timer_1.speed_mode = LEDC_HIGH_SPEED_MODE;
        timer_1.timer_num = LEDC_TIMER_0;
        timer_1.clk_cfg = LEDC_AUTO_CLK;
        timer_1.duty_resolution = LEDC_TIMER_14_BIT;
        timer_1.freq_hz = 4880;    //4.88 [kHz]
        ledc_timer_config(&timer_1);
        //channel conf
        ledc_channel_config_t channel_1{};
        channel_1.gpio_num = GPIO_NUM_0;
        channel_1.speed_mode = timer_1.speed_mode;
        channel_1.channel = LEDC_CHANNEL_0;
        channel_1.intr_type = LEDC_INTR_DISABLE;
        channel_1.timer_sel = timer_1.timer_num;
        channel_1.duty = 0;
        ledc_channel_config(&channel_1);
        //
        enum Mode {pulse, steady};
        Mode mode = pulse;
        //
        uint8_t pwm{10};
        //loop
        while (1) {
            switch(mode) {
                //pulse
                case Mode::pulse:
                _pulse(&timer_1, &channel_1);
                break;
                //steady
                case Mode::steady:
                _steady(&pwm, &timer_1, &channel_1);
                break;
                //default
                default:
                break;
            }
        }
    }
}