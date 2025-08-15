#pragma once

#include <string>
#include <vector>
#include <esp_afe_sr_iface.h>
#include <freertos/event_groups.h>
#include <functional>
#include <cstddef>


class WakeWordHal
{
private:
    /* data */

public:
    WakeWordHal(/* args */);
    virtual ~WakeWordHal() = default;
    virtual void start();
    virtual void stop();

    virtual size_t get_feed_size(int channel_num);
    virtual void feed(const std::vector<int16_t>& pcm);

    inline void set_wake_word_detected_ok_callback(std::function<void(const std::string& wake_word)> callback) {
        wake_word_detected_ok_callback_ = callback;
    }

    bool is_running();

protected:
    std::function<void(const std::string& wake_word)> wake_word_detected_ok_callback_;
    std::vector<std::string> wake_word_name_list_;
    esp_afe_sr_iface_t *afe_iface_handle_ = nullptr;
    esp_afe_sr_data_t *afe_sr_data_ = nullptr;
    EventGroupHandle_t event_group_;
    uint32_t WAKE_WORD_RUNNING_EVENT = (1 << 0);
};