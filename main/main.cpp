#include <bits/shared_ptr.h>
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"

#include "sd_card.h"
#include "wav_recorder.h"
#include "app.h"


extern "C" void app_main(void)
{
    std::printf("Hello, AI ESP32-S3!\n");
    App::GetInstance().run();
}
