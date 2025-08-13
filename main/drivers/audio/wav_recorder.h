#pragma once
#include "file_interface.h"
#include "driver/i2s_std.h"
#include <memory>


class WavRecorder {
public:
    explicit WavRecorder();

    esp_err_t record(uint16_t seconds);

private:

};