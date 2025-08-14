#pragma once
#include <string>
#include "board.h"
#include <functional>

class ProtocolAdapter
{
private:
    /* data */

protected:
    int sample_rate_ = 16000;
    int channels_ = 1;
    int frame_duration_ms_ = 60;
    std::string session_id_;
    std::function<void()> server_set_param_callback_;
    std::function<void(std::vector<int8_t> &&data)> audio_msg_callback_;

    virtual bool send_text(const std::string& text) = 0;

public:

    ProtocolAdapter(/* args */);
    ~ProtocolAdapter();

    virtual bool open_server_channel() = 0;

    virtual void send_audio(const std::vector<int8_t>& data) = 0;

    virtual bool send_listening_start(bool is_auto);
    virtual bool send_listening_stop();

    inline int get_server_frame_duration_ms() const {
        return frame_duration_ms_;
    }

    inline int get_server_sample_rate() const {
        return sample_rate_;
    }

    inline const std::string get_session_id() const {
        return session_id_;
    }

    inline int get_server_channel() const {
        return channels_;
    }

    inline void server_set_param(std::function<void()> callback) {
        server_set_param_callback_ = callback;
    }

    inline void get_audio_data(std::function<void(std::vector<int8_t>&& data)> callback) {
        audio_msg_callback_ = callback;
    }

};


