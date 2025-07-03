#include <cstdio>
#include <driver/i2s_std.h>
#include <driver/i2s_common.h>
#include <esp_log.h>
#include <esp_err.h>
#include <driver/gpio.h>




#define EXAMPLE_RECV_BUF_SIZE     (2400)
#define EXAMPLE_SAMPLE_RATE       (16000)
#define EXAMPLE_MCLK_MULTIPLE     (256) // If not using 24-bit data width, 256 should be enough
#define EXAMPLE_MCLK_FREQ_HZ      (EXAMPLE_SAMPLE_RATE * EXAMPLE_MCLK_MULTIPLE)
#define EXAMPLE_VOICE_VOLUME      (80)
#define I2S_DO_IO                 (GPIO_NUM_45)
#define EXAMPLE_I2S_MCK_IO        GPIO_NUM_38
#define EXAMPLE_I2S_BCK_IO        GPIO_NUM_14
#define EXAMPLE_I2S_WS_IO         GPIO_NUM_13

class AudioES8311
{
private:
    esp_err_t i2s_driver_init(void);
    esp_err_t es8311_codec_init(void);
    i2s_chan_handle_t tx_handle = NULL;
public:
    AudioES8311();
    ~AudioES8311();
    esp_err_t enable();
    esp_err_t disable();
    esp_err_t output(void *data, size_t size);
    void i2s_music();
};