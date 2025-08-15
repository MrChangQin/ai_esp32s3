#pragma once
#include <cstdio>
#include <list>
#include <vector>
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "work_task.h"
#include "file_interface.h"
#include "sd_card.h"
#include <opus_encoder.h>
#include <opus_decoder.h>
#include "wav_recorder.h"
#include "board.h"
#include "protocol_adapter.h"
#include "setting.h"
#include "lwip/def.h"
#include <esp_log.h>
#include <opus_resampler.h>

#ifdef CONFIG_PROTOCOL_TYPE_WEBSOCKET
#include "websocket_protocol.h"
#endif

#ifdef CONFIG_WAKE_WORD_DETECT_TYPE
#include "wake_word_detect.h"
#endif

#ifdef CONFIG_CMD_WORD_DETECT_TYPE
#include "cmd_word_detect.h"
#endif

#include "audio_hal.h"
#include "p3.h"
#include "wake_word_hal.h"
#include "input_audio_process.h"


// 单例模式
class App {
private:
    WorkTask* work_task = nullptr;

    std::unique_ptr<OpusEncoderWrapper> opus_encoder_;
    std::unique_ptr<OpusDecoderWrapper> opus_decoder_;

    OpusResampler input_resampler_;
    OpusResampler output_resampler_;
    OpusResampler ref_resampler_;

    std::unique_ptr<ProtocolAdapter> protocol_;
    void set_opus_param(int sample_rate, int frame_duration_ms, int channels);

    std::list<std::vector<uint8_t>> opus_queue_;
    std::condition_variable opus_queue_cv_;

    std::mutex pcm_mutex_;

    TaskHandle_t audio_task_handle_ = nullptr;

    void audio_task_loop();

    void audio_input_process(AudioHAL* audio_);
    void audio_output_process(AudioHAL* audio_);

    bool get_audio_pcm_resample(std::vector<int16_t> &pcm_data, int target_sample_rate, int samples, bool ref=true);

    std::unique_ptr<WakeWordHal> wake_word_hal_ = nullptr;

    std::mutex task_mutex;
    std::condition_variable task_condition_variable_;
    ListFunction task_list;
    uint32_t task_count;

    std::unique_ptr<InputAudioProcess> input_audio_process_ = nullptr;

    App();
    ~App();

public:
    static App& GetInstance() {
        static App instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    App(App const&) = delete;
    App& operator=(App const&) = delete;
    void run();

    void print_all_tasks();

    void play_p3_audio(const std::string_view& p3_sound_lable);

    void play_number(int number);

    void add_task(FuncVoid task);

};
