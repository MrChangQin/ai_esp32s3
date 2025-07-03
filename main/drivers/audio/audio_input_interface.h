// file_interface.h
#pragma once
#include <cstdio>
#include <esp_err.h>


class AudioInputInterface {
public:
    virtual ~AudioInputInterface() = default; // 基类的析构函数一定要是虚（virtual）函数，这样在销毁派生类的对象时才能正确调用派生类的析构函数

    virtual esp_err_t enable() = 0;  // 对应i2s enable
    virtual esp_err_t disable() = 0;    // 对应i2s disable

    // 原接口保留（可选，推荐改用新接口）
    virtual esp_err_t read(void* dest, size_t size, size_t *bytes_read) = 0;  // 对应i2s read

};