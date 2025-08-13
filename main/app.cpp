#include "app.h"


App::App() { 
    work_task = new WorkTask(4096 * 2);
    opus_encoder_ = std::make_unique<OpusEncoderWrapper>(16000, 1, 60);
    opus_decoder_ = std::make_unique<OpusDecoderWrapper>(16000, 1, 60);

}

App::~App() {
    
}

void App::print_all_tasks() {
    char task_list_buffer[1024] = {0};
    vTaskList(task_list_buffer);
    printf("Task List: \n%s\n", task_list_buffer);
}

void App::audio_output_process(AudioHAL* audio_) {
    std::unique_lock<std::mutex> lock(pcm_mutex_);
    if (opus_packets_.empty()) {
        return;
    }
    auto opus = std::move(opus_packets_.front());
    opus_packets_.pop_front();
    lock.unlock();
    audio_->enable_output();

    work_task->add_task([this, audio_, opus = std::move(opus)]() mutable {
        std::vector<int16_t> pcm;
        if (!opus_decoder_->Decode(std::move(opus), pcm)) {
            return;
        }
        audio_->output_data(pcm);
    });
}

void App::audio_task_loop() {
    auto audio_ = Board::GetInstance().GetAudioHAL();
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(30));
        App::audio_output_process(audio_);
    }
}

void App::run() { 
    xTaskCreatePinnedToCore([](void* arg) {
        App* app = (App*)arg;
        app->audio_task_loop();
        vTaskDelete(NULL);
    }, "audio", 4096 * 2, this, 8, &audio_task_handle_, 0);

    auto& board = Board::GetInstance();

    board.start_net();
    protocol_ = std::make_unique<WebsocketProtocol>();
    protocol_->open_server_channel();

    int is_enter_wifi_config;
    Settings setting = Settings(SettingNameSpace::WIFI, true);
    setting.set_int(SettingKey::IS_ENTER_WIFI_CONFIG, 2);
    is_enter_wifi_config = setting.get_int(SettingKey::IS_ENTER_WIFI_CONFIG, -1);

    std::cout << "is_enter_wifi_config: " << is_enter_wifi_config << "\n";

    // WavRecorder recorder;
    // recorder.record(10);

    auto audio_ = Board::GetInstance().GetAudioHAL();

    audio_->enable_input();
    std::printf("开始说话\n");

    vTaskDelay(pdMS_TO_TICKS(1000));

    {   
        std::unique_lock<std::mutex> lock(pcm_mutex_);

        for (size_t i = 0; i < 50; i++) 
        {
            std::vector<int16_t> pcm(960 * 2); // 两路mic data
            audio_->input_data(pcm); // 

            auto mic_pcm = std::vector<int16_t>(pcm.size() / 2);

            for (size_t i = 0, j = 0; i < mic_pcm.size(); ++i, j += 2) {  // 获取单声道数据
                mic_pcm[i] = pcm[j];
            }

            work_task->add_task([this, mic_pcm = std::move(mic_pcm)]() mutable {
                opus_encoder_->Encode(std::move(mic_pcm), [this](std::vector<uint8_t>&& opus){
                    opus_packets_.emplace_back(std::move(opus));
                });
            });
        }
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        print_all_tasks();
    }
}
