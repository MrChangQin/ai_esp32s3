#pragma once
#include "wake_word_hal.h"
#include <audio_hal.h>


class WakeWordDetect: public WakeWordHal
{
public:
    WakeWordDetect(AudioHAL* audio_hal);
    ~WakeWordDetect();

    // virtual size_t get_feed_size(int channel_num) override;
    // virtual void feed(const std::vector<int16_t>& pcm) override;

private:
    void wake_word_detect_task();
};