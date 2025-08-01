#pragma once
#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include <list>
#include <vector>
#include "work_task.h"
#include "file_interface.h"
#include "sd_card.h"
#include <opus_encoder.h>
#include <opus_decoder.h>
#include "wav_recorder.h"
#include "board.h"


// 单例模式
class App {
private:
    WorkTask* work_task = NULL;
    std::unique_ptr<OpusEncoderWrapper> opus_encoder_;
    std::unique_ptr<OpusDecoderWrapper> opus_decoder_;
    std::list<std::vector<uint8_t>> opus_packets_;  // 每个vector代表一个opus包
    std::mutex pcm_mutex_;


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
