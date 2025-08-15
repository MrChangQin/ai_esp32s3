#include "app.h"


App::App() { 
    work_task = new WorkTask(4096 * 8);
    opus_encoder_ = std::make_unique<OpusEncoderWrapper>(16000, 1, 60);
    opus_decoder_ = std::make_unique<OpusDecoderWrapper>(16000, 1, 60);

    #ifdef CONFIG_PROTOCOL_TYPE_WEBSOCKET
    protocol_ = std::make_unique<WebsocketProtocol>();
    #endif

    if (protocol_ == nullptr) {
        ESP_LOGE(TAG, "protocol_ is nullptr");
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
    auto audio_ = Board::GetInstance().GetAudioHAL();
    output_resampler_.Configure(opus_decoder_->sample_rate(), audio_->get_output_sample_rate());

    ref_resampler_.Configure(audio_->get_input_sample_rate(), 16000);
    input_resampler_.Configure(audio_->get_input_sample_rate(), 16000);
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
    if (opus_queue_.empty()) {
        opus_queue_cv_.notify_one();
        return;
    }
    auto opus = std::move(opus_queue_.front());
    opus_queue_.pop_front();
    lock.unlock();
    audio_->enable_output();

    work_task->add_task([this, audio_, opus = std::move(opus)]() mutable {
        std::vector<int16_t> pcm;
        if (!opus_decoder_->Decode(std::move(opus), pcm)) {
            return;
        }

        int target_size = output_resampler_.GetOutputSamples(pcm.size());
        std::vector<int16_t> resampled_pcm(target_size);
        output_resampler_.Process(pcm.data(), pcm.size(), resampled_pcm.data());
        pcm = std::move(resampled_pcm),
        audio_->output_data(pcm);
    });
}

void App::play_number(int number) {
    if (number < 0) {
        number = -number; // 处理负数（可选）
    }
    // 将数字转为字符串，方便逐字符处理
    std::string num_str = std::to_string(number);
    for (char c : num_str) {
        int digit = c - '0'; // 字符转数字（0-9）
        if (digit >= 0 && digit <= 9) {
            play_p3_audio(P3SoundLable::DIGIT_AUDIO[digit]); // 通过数组直接访问
        }
    }
}

void App::play_p3_audio(const std::string_view& p3_sound_lable) {

    {
        std::unique_lock<std::mutex> lock(pcm_mutex_);
        opus_queue_cv_.wait(lock, [this]() {
            return opus_queue_.empty();
        });
    }

    work_task->wait_work_task_completion();

    const char* data = p3_sound_lable.data();
    size_t size = p3_sound_lable.size();

    for (const char* p = data; p < data + size;) {
        auto p3_frame_header = (P3HeaderStructure*)p;
        auto payload_size = ntohs(p3_frame_header->payload_size);

        std::vector<uint8_t> opus(payload_size);
        memcpy(opus.data(), p3_frame_header->payload, payload_size);

        p += sizeof(P3HeaderStructure) + payload_size;
        std::lock_guard<std::mutex> lock(pcm_mutex_);
        opus_queue_.emplace_back(std::move(opus));
    }
}

bool App::get_audio_pcm_resample(std::vector<int16_t> &pcm_data, int target_sample_rate, int samples, bool ref) {

    bool is_target = true;
    auto audio_ = Board::GetInstance().GetAudioHAL();
    // 处理参考通道
    // if (audio_->is_input_ref()) {
    //     samples = samples * 2;
    // }

    // 如果目标采样率和麦克风不一致则需要重采样
    if (audio_->get_input_sample_rate() != target_sample_rate) {
        samples = samples * audio_->get_input_sample_rate()/target_sample_rate;
        is_target = false;
    }

    pcm_data.resize(samples);
    if (!audio_->input_data(pcm_data)) {
        return false;
    }

    // 如果音频未设置参考通道(单通道)
    if (!audio_->is_input_ref())
    {
        if (!is_target)
        {
            auto out_size = input_resampler_.GetOutputSamples(pcm_data.size());
            auto target_pcm = std::vector<int16_t>(out_size);
            input_resampler_.Process(pcm_data.data(), pcm_data.size(), target_pcm.data());
            pcm_data = std::move(target_pcm);
        }
        return true;
    }

    auto mic_pcm = std::vector<int16_t>(pcm_data.size() / 2);
    auto ref_pcm = std::vector<int16_t>(pcm_data.size() / 2);

    for (int i = 0, j = 0; i < mic_pcm.size(); i++, j += 2)
    {
        mic_pcm[i] = pcm_data[j];
        ref_pcm[i] = pcm_data[j + 1];
    }

    auto out_size = input_resampler_.GetOutputSamples(mic_pcm.size());
    auto mic_target_pcm = std::vector<int16_t>(out_size);
    input_resampler_.Process(mic_pcm.data(), mic_pcm.size(), mic_target_pcm.data());

    if (!ref) {
        if (!is_target) {

            pcm_data = std::move(mic_target_pcm);
        }
        else {
            pcm_data = std::move(mic_pcm);
        }
        return true;
    }

    //处理参考通道
    auto ref_target_pcm = std::vector<int16_t>(ref_resampler_.GetOutputSamples(ref_pcm.size()));
    ref_resampler_.Process(ref_pcm.data(), ref_pcm.size(), ref_target_pcm.data());
    pcm_data.resize(mic_target_pcm.size() + ref_target_pcm.size());
    for (size_t i = 0, j = 0; i < mic_target_pcm.size(); ++i, j += 2) {
        pcm_data[j] = mic_target_pcm[i];
        pcm_data[j + 1] = ref_target_pcm[i];
    }
    return true;

}

void App::set_opus_param(int sample_rate, int frame_duration_ms, int channels) {
    if (opus_encoder_->sample_rate() != sample_rate
        || opus_encoder_->duration_ms() != frame_duration_ms)
    {
        opus_encoder_.reset();
        opus_encoder_ = std::make_unique<OpusEncoderWrapper>(sample_rate, channels, frame_duration_ms);
    }

    if (opus_decoder_->sample_rate() != sample_rate
        || opus_decoder_->duration_ms() != frame_duration_ms)
    {
        ESP_LOGW(TAG, "Opus decoder sample rate or frame duration changed" );
        opus_decoder_.reset();
        opus_decoder_ = std::make_unique<OpusDecoderWrapper>(sample_rate, channels, frame_duration_ms);
    }

    auto audio_ = Board::GetInstance().GetAudioHAL();
    if (opus_encoder_->sample_rate() != audio_->get_input_sample_rate())
    {
        input_resampler_.Configure(audio_->get_input_sample_rate(), opus_encoder_->sample_rate());
    }
    if (opus_decoder_->sample_rate() != audio_->get_output_sample_rate())
    {
        output_resampler_.Configure(opus_decoder_->sample_rate(), audio_->get_output_sample_rate());
    }
}

void App::audio_task_loop()
{
    auto audio_ = Board::GetInstance().GetAudioHAL();
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(30));
        App::audio_input_process(audio_);
        App::audio_output_process(audio_);
    }
}

void App::audio_input_process(AudioHAL* audio_) {
#ifdef CONFIG_WAKE_WORD_DETECT_TYPE || CONFIG_CMD_WORD_DETECT_TYPE
    if (wake_word_hal_->is_running()) {
        int samples = wake_word_hal_->get_feed_size(2);
        std::vector<int16_t> pcm_data(samples);
        get_audio_pcm_resampl(pcm_data, 16000, samples);
        wake_word_hal_->feed(pcm_data);
    }
#endif
}

void App::run() { 

    auto audio_ = Board::GetInstance().GetAudioHAL();

#ifdef CONFIG_WAKE_WORD_DETECT_TYPE
wake_word_hal_ = std::make_unique<WakeWordDetect>(audio_);
#endif
#ifdef CONFIG_CMD_WORD_DETECT_TYPE
wake_word_hal_ = std::make_unique<WakeWordHal>(audio_);
#endif
#ifdef CONFIG_WAKE_WORD_DETECT_TYPE || CONFIG_CMD_WORD_DETECT_TYPE
    wake_word_hal_->set_wake_word_detected_ok_callback([this](const std::string& wake_word) {
        std::printf("唤醒词：%s\n", wake_word.c_str());
        wake_word_hal_->stop();
    });
    wake_word_hal_->start();
#endif

    xTaskCreatePinnedToCore([](void* arg) {
        App* app = (App*)arg;
        app->audio_task_loop();
        vTaskDelete(NULL);
    }, "audio", 4096 * 2, this, 8, &audio_task_handle_, 0);

    auto& board = Board::GetInstance();

    board.start_net();

    protocol_->server_set_param([this]() {
        set_opus_param(
            protocol_->get_server_sample_rate(), 
            protocol_->get_server_frame_duration_ms(),
            protocol_->get_server_channel()
        );
    });

    protocol_->get_audio_data([this](std::vector<uint8_t>&& data) {
        std::lock_guard<std::mutex> lock(pcm_mutex_);
        opus_queue_.emplace_back(std::move(data));
    });

    protocol_->open_server_channel();


#if 0
    int is_enter_wifi_config;
    Settings setting = Settings(SettingNameSpace::WIFI, true);
    setting.set_int(SettingKey::IS_ENTER_WIFI_CONFIG, 2);
    is_enter_wifi_config = setting.get_int(SettingKey::IS_ENTER_WIFI_CONFIG, -1);
#endif


    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        // print_all_tasks();
    }
}
