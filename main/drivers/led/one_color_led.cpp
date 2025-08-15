#include "one_color_led.h"
#include <iostream>
#include <cmath>


#define LEDC_SPEED_MODE LEDC_LOW_SPEED_MODE
#define LEDC_TIMER_BIT LEDC_TIMER_13_BIT

OneColorLed::~OneColorLed() { }

OneColorLed::OneColorLed(gpio_num_t pin, ledc_timer_t timer_num, ledc_channel_t channel, bool is_output_invert) {
    ledc_timer_config_t ledc_timer = {};
    // 设置PWM的分辨率，这里设置13位，即0-8191，也就是调光的细腻度
    ledc_timer.duty_resolution = LEDC_TIMER_BIT;
    // PWM的频率，频率过低会导致LED闪烁，高于200Hz的闪烁基本人眼无法察觉
    ledc_timer.freq_hz = 4000;
    // 选择低速模式
    ledc_timer.speed_mode = LEDC_SPEED_MODE;
    ledc_timer.timer_num = timer_num;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_.channel = channel;
    // 初始化时的占空比范围0-8191（针对LEDC_TIMER_13_BIT），设置0表示为关闭状态
    ledc_channel_.duty = 0;
    ledc_channel_.gpio_num = pin;
    ledc_channel_.speed_mode = LEDC_SPEED_MODE;
    // 相位偏移，我们不需要相位偏移，所以设置为0
    ledc_channel_.hpoint = 0;

    // 使用的定时器编号 0-3
    ledc_channel_.timer_sel = timer_num;
    ledc_channel_.flags.output_invert = is_output_invert ? 0x01 : 0x00;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_));

    //安装LEDC的渐变服务
    ledc_fade_func_install(0);

    ledc_cbs_t ledc_callbacks = {
        .fade_cb = [](const ledc_cb_param_t *param, void *user_arg) -> bool {
            if (param->event == LEDC_FADE_END_EVT) {
                auto led = static_cast<OneColorLed*>(user_arg);
                led->fade_count_--;
                if (led->fade_count_ == 0) {
                    led->stop_fade();
                } else {
                    led->is_up_ = !led->is_up_;
                    ledc_set_fade_with_time(led->ledc_channel_.speed_mode,
                                            led->ledc_channel_.channel,
                                            led->is_up_ ? led->fade_target_duty_ : 0,
                                            led->fade_time_ms_);
                    ledc_fade_start(led->ledc_channel_.speed_mode, led->ledc_channel_.channel, LEDC_FADE_NO_WAIT);
                }
            }
            return true;
        }
    };
    ledc_cb_register(ledc_channel_.speed_mode, ledc_channel_.channel, &ledc_callbacks, this);
}

void OneColorLed::start_fade(uint8_t fade_brightness, int fade_count, int fade_time_ms, bool is_up)
{
    std::lock_guard<std::mutex> lock(mutex_);
    ledc_fade_stop(ledc_channel_.speed_mode, ledc_channel_.channel);

    fade_target_duty_ = (1 << LEDC_TIMER_BIT) * fade_brightness / 100;
    fade_count_ = fade_count * 2;
    fade_time_ms_ = fade_time_ms;
    is_up_ = is_up;

    ledc_set_fade_with_time(ledc_channel_.speed_mode, ledc_channel_.channel, is_up ? fade_target_duty_ : 0, fade_time_ms);
    ledc_fade_start(ledc_channel_.speed_mode, ledc_channel_.channel, LEDC_FADE_NO_WAIT);
}

void OneColorLed::stop_fade() {
    std::lock_guard<std::mutex> lock(mutex_);
    ledc_fade_stop(ledc_channel_.speed_mode, ledc_channel_.channel);
}

void OneColorLed::off_led() {
    std::lock_guard<std::mutex> lock(mutex_);
    ledc_fade_stop(ledc_channel_.speed_mode, ledc_channel_.channel);

    ledc_set_duty(LED_SPEED_MODE, ledc_channel_.channel, 0);
    ledc_update_duty(LED_SPEED_MODE, ledc_channel_.channel);
}

void OneColorLed::on_led() {
    std::lock_guard<std::mutex> lock(mutex_);
    ledc_fade_stop(ledc_channel_.speed_mode, ledc_channel_.channel);

    ledc_set_duty(LED_SPEED_MODE, ledc_channel_.channel, (1 << LEDC_TIMER_BIT) - 1);
    ledc_update_duty(LED_SPEED_MODE, ledc_channel_.channel);
}

void OneColorLed::set_brightness(uint8_t brightness) {
    std::lock_guard<std::mutex> lock(mutex_);
    ledc_fade_stop(ledc_channel_.speed_mode, ledc_channel_.channel);

    uint32_t duty = ((1 << LEDC_TIMER_BIT) - 1) * brightness / 100;
    ledc_set_duty(LED_SPEED_MODE, ledc_channel_.channel, duty);
    ledc_update_duty(LED_SPEED_MODE, ledc_channel_.channel);
}
