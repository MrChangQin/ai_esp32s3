#include "wake_word_hal.h"


WakeWordHal::WakeWordHal()
{
    event_group_ = xEventGroupCreate();
}

void WakeWordHal::start()
{
    xEventGroupSetBits(event_group_, WAKE_WORD_RUNNING_EVENT);
}

void WakeWordHal::stop()
{
    xEventGroupClearBits(event_group_, WAKE_WORD_RUNNING_EVENT);
}

bool WakeWordHal::is_running() {
    return (xEventGroupGetBits(event_group_) & WAKE_WORD_RUNNING_EVENT);
}

void WakeWordHal::feed(const std::vector<int16_t>& pcm) {
    if (afe_sr_data_ == nullptr) {
        return;
    }
    afe_iface_handle_->feed(afe_sr_data_, pcm.data());
}

size_t WakeWordHal::get_feed_size(int channel_num) {
    if (afe_sr_data_ == nullptr) {
        return 0;
    }
    return afe_iface_handle_->get_feed_chunksize(afe_sr_data_) * channel_num;
}