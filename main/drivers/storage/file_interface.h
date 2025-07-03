#pragma once
#include <cstddef>
#include "esp_err.h"

enum class SeekMode {
    Set = SEEK_SET,  // Set the file position to the specified offset
    Cur = SEEK_CUR,  // Set the file position to the current position plus the offset
    End = SEEK_END  // Set the file position to the end of the file plus the offset
};

class FileInterface {
public:
    virtual ~FileInterface() = default; // Virtual destructor for proper cleanup

    virtual esp_err_t open(const char *filename, const char *mode) = 0;
    virtual esp_err_t close() = 0;

    virtual esp_err_t read_file(char *output, size_t output_size) = 0;
    virtual esp_err_t write_file(const char *data, size_t size) = 0;
    virtual esp_err_t read_line(int line_num, char *output, size_t output_size) = 0;
    virtual esp_err_t seek(size_t offset, SeekMode mode) = 0;

};