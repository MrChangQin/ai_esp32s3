#include <bits/shared_ptr.h>
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include <cJSON.h>

#include "sd_card.h"
#include "wav_recorder.h"
#include "app.h"
#include "ble_lib.h"


#define TAG "MAIN"

extern "C" void app_main(void)
{
    std::printf("Hello, AI ESP32-S3!\n");
    ESP_ERROR_CHECK(nvs_flash_init());

    App::GetInstance().run();
}
