#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include <rom/ets_sys.h>

extern "C" {
    void app_main () {
        //timer conf
        ledc_timer_config_t timer_1{};
        timer_1.speed_mode = LEDC_HIGH_SPEED_MODE;
        timer_1.timer_num = LEDC_TIMER_0;
        timer_1.clk_cfg = LEDC_AUTO_CLK;
        timer_1.duty_resolution = LEDC_TIMER_12_BIT;
        timer_1.freq_hz = 15000;    //15 [kHz]
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
        size_t pwm{};
        bool condition{true}; 
        while(true) {
            if (condition) {
                ledc_set_duty(channel_1.speed_mode, channel_1.channel, pwm++);
                ledc_update_duty(channel_1.speed_mode, channel_1.channel);
                ets_delay_us(1000);  //1 [ms]
                if (pwm >= 4095) condition = !condition;
            }
            else {
                ledc_set_duty(channel_1.speed_mode, channel_1.channel, pwm--);
                ledc_update_duty(channel_1.speed_mode, channel_1.channel);
                ets_delay_us(1000);  //1 [ms]
                if (pwm <= 0) condition = !condition;
            }
        }
    }
}