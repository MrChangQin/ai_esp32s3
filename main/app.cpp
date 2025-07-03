#include "app.h"


App::App()
{ 
    work_task = new WorkTask(4096 * 2);

    opus_encoder_ = std::make_unique<OpusEncoderWrapper>(16000, 1, 60);
    audio_input_ = std::make_unique<AudioEs7210>(16000, 1);
    file_ = std::make_unique<SdCard>();
    opus_decoder_ = std::make_unique<OpusDecoderWrapper>(16000, 1, 60);
    audio_output_ = std::unique_ptr<AudioES8311>();
}

App::~App()
{
}

void App::run()
{ 

    ESP_ERROR_CHECK(file_->open("test.opus", "wb"));
    std::printf("开始说话\n");
    // 
    audio_input_->enable();
    vTaskDelay(pdMS_TO_TICKS(1000));

    for (size_t i = 0; i < 50; i++) {

        std::vector<int16_t> pcm(960);

        audio_input_->read(pcm.data(), pcm.size()*sizeof(int16_t), nullptr);

        work_task->add_task([this, pcm = std::move(pcm)]() mutable {
            opus_encoder_->Encode(std::move(pcm), [this](std::vector<uint8_t>&& opus){
                opus_packets_.emplace_back(std::move(opus));
                // // 音频编码完成
                // uint32_t frame_size = static_cast<uint32_t>(opus.size());
                // //2、转为大端序
                // uint32_t frame_size_big_endian = htonl(frame_size); //转为大端序 0x12345678·->·0x78563412
                // //3.先写入帧长度（4字节）
                // file_->write_file(reinterpret_cast<const char*>(&frame_size_big_endian), sizeof(frame_size_big_endian));
                // //4.再写入编码后的数据
                // file_->write_file(reinterpret_cast<const char*>(opus.data()), opus.size());
            });
        });
    }

    // 延时（必要，等待后台任务执行完成）
    vTaskDelay(pdMS_TO_TICKS(1000));
    audio_input_->disable();
    // file_->close();
    std::printf("结束说话\n");

    audio_output_->disable();
    audio_output_->enable();
    vTaskDelay(pdMS_TO_TICKS(1000));


    // 释放内存, 进行解码
    for (auto& opus : opus_packets_) {
        work_task->add_task([this, opus = std::move(opus)]() mutable {
            std::vector<int16_t> decoded_pcm;
            opus_decoder_->Decode(std::move(opus), decoded_pcm);

            std::lock_guard<std::mutex> lock(pcm_mutex_);
            const uint8_t* data_ptr = (const uint8_t*) decoded_pcm.data();
            audio_output_->output((char *)data_ptr, decoded_pcm.size() * sizeof(int16_t));
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
    // vTaskList(task_list_buffer);
    printf("Task List: \n%s\n", task_list_buffer);
}
