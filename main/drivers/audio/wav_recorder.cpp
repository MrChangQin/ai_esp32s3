#include "wav_recorder.h"
#include "format_wav.h"
#include <esp_timer.h>
#include <esp_check.h>
#include "board.h"



#define TAG "WavRecorder"
#define EXAMPLE_I2S_SAMPLE_RATE 16000
#define EXAMPLE_I2S_CHAN_NUM 2
#define EXAMPLE_I2S_SAMPLE_BITS 16


WavRecorder::WavRecorder()
{

}

//TODO:存在问题
esp_err_t WavRecorder::record(uint16_t seconds) {

    esp_err_t ret = ESP_OK;
    // 计算音频数据大小
    uint32_t byte_rate = EXAMPLE_I2S_SAMPLE_RATE * EXAMPLE_I2S_CHAN_NUM * EXAMPLE_I2S_SAMPLE_BITS / 8;
    uint32_t wav_size = seconds * byte_rate;

    // 创建初始WAV头部（数据大小暂时设为0）
    wav_header_t wav_header = WAV_HEADER_PCM_DEFAULT(wav_size, EXAMPLE_I2S_SAMPLE_BITS,
                                                    EXAMPLE_I2S_SAMPLE_RATE,
                                                    EXAMPLE_I2S_CHAN_NUM);

    auto m_file = Board::GetInstance().GetFileInterface();
    auto audio_ = Board::GetInstance().GetAudioHAL();  

    audio_->enable_input();

    m_file->open("test.wav", "w+");

    // 1. 写入WAV头部
    if (m_file->write_file((char *)&wav_header, sizeof(wav_header)) != ESP_OK) {
        ESP_LOGE(TAG, "写入WAV头部失败");
        return ESP_FAIL;
    }

    // 2. 实时录音并分段写入数据
    size_t wav_written = 0;
    static int16_t i2s_readraw_buff[8192];
    std::vector<int16_t> pcm_buffer(i2s_readraw_buff, i2s_readraw_buff + sizeof(i2s_readraw_buff)/sizeof(int16_t));
    uint32_t last_printed_sec = 0;

    // 使能通道
    audio_->enable_input();

    while (wav_written < wav_size) {
        // 使用 input_data 方法替代直接调用 read
        if (audio_->input_data(pcm_buffer)) {
            size_t bytes_read = pcm_buffer.size() * sizeof(int16_t); // 每个样本是int16_t类型

            // 进度打印（每秒打印一次）
            uint32_t current_sec = wav_written / byte_rate;
        
            if (current_sec > last_printed_sec) {
                last_printed_sec = current_sec;
                ESP_LOGI(TAG, "Recording: %" PRIu32 " s/%ds", current_sec, seconds);
            }
            if (m_file->write_file((char *)pcm_buffer.data(), bytes_read) != ESP_OK) {
                ESP_LOGE(TAG, "写入WAV数据失败");
                goto err;
            }
            wav_written += bytes_read;
        } else {
            ESP_LOGW(TAG, "未能读取音频数据");
        }
    }
    m_file->close();
    printf("WAV文件写入完成\n");

err:
    audio_->disable_input();
    return ret;
    
}
