#include "wake_word_detect.h"
#include <esp_log.h>
#include <string>
#include <sstream>
#include <esp_afe_sr_models.h>


#define TAG "WakeWordDetect"

WakeWordDetect::WakeWordDetect(AudioHAL* audio_hal)
{
    //加载模型
    srmodel_list_t *models = esp_srmodel_init("model");

    /**提取模型的唤醒词 */
    for (int i = 0; i < models->num; i++) {
        if (esp_srmodel_filter(models, ESP_WN_PREFIX, NULL) != NULL) {
            auto words = esp_srmodel_get_wake_words(models, models->model_name[i]);
            // 这里获取到的唤醒词如果有多个使用';'分隔的
            std::stringstream ss_words(words);
            std::string wake_word_name;
            while (std::getline(ss_words, wake_word_name, ';')) {
                wake_word_name_list_.push_back(wake_word_name);
                ESP_LOGI(TAG, "唤醒词: %s", wake_word_name.c_str());
            }
        }
    }

    std::string input_format;
    input_format.push_back('M');
    if (audio_hal->is_input_ref()) {
        input_format.push_back('R');
    }

    afe_config_t* afe_config = afe_config_init(input_format.c_str(), models, AFE_TYPE_SR, AFE_MODE_HIGH_PERF);
    afe_config->aec_init = audio_hal->is_input_ref();
    afe_config->aec_mode = AEC_MODE_SR_HIGH_PERF;
    afe_config->afe_perferred_core = 1;
    afe_config->afe_perferred_priority = 1;
    afe_config->vad_init = true;
    afe_config->vad_mode = VAD_MODE_0;
    afe_config->vad_min_noise_ms = 100;
    afe_config->memory_alloc_mode = AFE_MEMORY_ALLOC_MORE_PSRAM;

    afe_iface_handle_ = esp_afe_handle_from_config(afe_config);
    afe_sr_data_ = afe_iface_handle_->create_from_config(afe_config);
    afe_config_free(afe_config);

    if (wake_word_name_list_.size() == 0) {
        ESP_LOGE(TAG, "please add wake word");
        return;
    }

    xTaskCreate([](void* arg) {
        auto this_ = (WakeWordDetect*)arg;
        this_->wake_word_detect_task();
        vTaskDelay(NULL);
    }, "wake_word_detect_task", 4096, this, 3, nullptr);
}

WakeWordDetect::~WakeWordDetect() {
    if (event_group_ != nullptr) {
        vEventGroupDelete(event_group_);
        event_group_ = nullptr;
    }
}

void WakeWordDetect::wake_word_detect_task() {
    auto fetch_size = afe_iface_handle_->get_fetch_chunksize(afe_sr_data_);
    auto feed_size = afe_iface_handle_->get_feed_chunksize(afe_sr_data_);

    while (true) {
        xEventGroupWaitBits(event_group_, WAKE_WORD_RUNNING_EVENT, pdFALSE, pdTRUE, portMAX_DELAY);
        auto res = afe_iface_handle_->fetch_with_delay(afe_sr_data_, portMAX_DELAY);
        if (res == nullptr || res->ret_value == ESP_FAIL) {
            continue;
        }

        if (res->wakeup_state == WAKENET_DETECTED) {
            if (wake_word_detected_ok_callback_) {
                wake_word_detected_ok_callback_(wake_word_name_list_[res->wake_word_index - 1]);
            }
            stop();
        }
    }
}

