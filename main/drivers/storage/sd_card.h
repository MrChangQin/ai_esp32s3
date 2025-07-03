#pragma once
#include <cstdio>
#include <cstring>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"

#include "file_interface.h"


#define TAG "SD_CARD"

#define SD_CARD_MOUNT_POINT "/sdcard"
#define SD_CARD_MAX_FILES 5
#define SD_CARD_PIN_CLK GPIO_NUM_14
#define SD_CARD_PIN_CMD GPIO_NUM_15
#define SD_CARD_PIN_D0  GPIO_NUM_2


class SdCard : public FileInterface {
private:
    FILE *m_file = nullptr; // Pointer to the file being accessed

public:
    SdCard();
    ~SdCard();

    esp_err_t open(const char *filename, const char *mode) override;
    esp_err_t close() override;

    esp_err_t write_file(const char *data, size_t size) override;
    esp_err_t read_file(char *output, size_t output_size) override;
    esp_err_t read_line(int line_num, char *output, size_t output_size) override;
    esp_err_t seek(size_t offset, SeekMode mode) override;

    esp_err_t sdCardWriteFile(const char *filename, const char *data, bool isAppend=false);
    esp_err_t sdCardReadFile(const char *filename, char *output, size_t output_size);
    esp_err_t sdCardReadLine(const char *filename, int line_num, char *output, size_t output_size);
};

