#include "app.h"


App::App()
{ 
    work_task = new WorkTask(4096 * 2);
    opus_encoder_ = std::make_unique<OpusEncoderWrapper>(16000, 1, 60);
    opus_decoder_ = std::make_unique<OpusDecoderWrapper>(16000, 1, 60);

}

App::~App()
{
}

void App::run()
{ 

    // WavRecorder recorder;
    // recorder.record(10);

    auto audio_ = Board::GetInstance().GetAudioHAL();

    audio_->enable_input();
    std::printf("开始说话\n");

    vTaskDelay(pdMS_TO_TICKS(1000));
    for (size_t i = 0; i < 50; i++) {

        std::vector<int16_t> pcm(960 * 2); // 两路mic data
        audio_->read(pcm.data(), pcm.size()); // 

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

    audio_->enable_output();

    // 释放内存, 进行解码
    for (auto& opus : opus_packets_) {
        work_task->add_task([this, opus = std::move(opus), audio_]() mutable {
            std::vector<int16_t> decoded_pcm;
            opus_decoder_->Decode(std::move(opus), decoded_pcm);

            std::lock_guard<std::mutex> lock(pcm_mutex_);
            const uint8_t* data_ptr = (const uint8_t*) decoded_pcm.data();
            audio_->write((int16_t *)data_ptr, decoded_pcm.size());
        });
    }


    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        print_all_tasks();
    }

}

void App::print_all_tasks() {
    char task_list_buffer[1024] = {0};
    vTaskList(task_list_buffer);
    printf("Task List: \n%s\n", task_list_buffer);
}
