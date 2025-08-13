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
#include "websocket_protocol.h"
#include "audio_hal.h"


// 单例模式
class App {
private:
    WorkTask* work_task = nullptr;
    std::unique_ptr<OpusEncoderWrapper> opus_encoder_;

    std::unique_ptr<OpusDecoderWrapper> opus_decoder_;

    std::unique_ptr<ProtocolAdapter> protocol_;

    std::list<std::vector<uint8_t>> opus_packets_;  // 每个vector代表一个opus包

    std::mutex pcm_mutex_;

    TaskHandle_t audio_task_handle_ = nullptr;

    void audio_task_loop();
    void audio_output_process(AudioHAL* audio_);

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

};
