#pragma once
#include <cstdint>

class Led {
public:
    virtual ~Led() = default;

/** @brief 打开LED，亮度设置为100%*/
    virtual void on_led() = 0;

/** @brief 关闭LED*/
    virtual void off_led() = 0;

/**
 * @brief 设置LED亮度
 * @param brightness 亮度值，范围0-100%
 */
    virtual void set_brightness(uint8_t brightness) = 0;

/**
 * @brief 开始LED渐变
 * @param fade_brightness 渐变亮度值，范围0-100%
 * @param fade_count 渐变次数
 * @param fade_time_ms 呼吸一次的时间ms
 * @param is_up 第一次渐变是由暗渐变到亮这个值为true，否则为false
 */
    virtual void start_fade(uint8_t fade_brightness, int fade_count, int fade_time_ms, bool is_up) = 0;

/**
 * @brief 停止LED渐变
 */
    virtual void stop_fade() = 0;

};

class NotLed : public Led {
public:
    virtual void off_led() override {}
    virtual void on_led() override {}
    virtual void set_brightness(uint8_t brightness) override {}

    virtual void start_fade(uint8_t fade_brightness, int fade_count, int fade_time_ms, bool is_up) override {}
    virtual void stop_fade() override {}
};