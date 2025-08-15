#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "led.h"
#include <driver/ledc.h>
#include <driver/gpio.h>
#include <mutex>


class OneColorLed : public Led {
public:
    /**
     * @brief 构造函数
     * @param pin gpio引脚
     * @param timer_num ledc定时器
     * @param channel ledc通道
     * @param is_output_invert pwm输出是否翻转,ture:翻转,false:不翻转
     */
    OneColorLed(gpio_num_t pin, ledc_timer_t timer_num, ledc_channel_t channel, bool is_output_invert);

    virtual void off_led() override;
    virtual void on_led() override;
    virtual void set_brightness(uint8_t brightness) override;

    virtual void start_fade(uint8_t fade_brightness, int fade_count, int fade_time_ms, bool is_up) override;
    virtual void stop_fade() override;

    ~OneColorLed();

private:
    // 互斥锁
    std::mutex mutex_;
    // 呼吸次数
    int fade_count_ = 0;
    // 呼吸的最高亮度的占空比值
    uint32_t fade_target_duty_ = 0;
    // 是否由暗到亮
    bool is_up = true;
    // 呼吸耗时时间从暗到亮或从亮到暗
    int fade_time_ms_ = 0;

    ledc_channel_config_t ledc_channel_ = {0};
};