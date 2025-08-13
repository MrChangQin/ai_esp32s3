#include "setting.h"
#include <esp_log.h>


Settings::Settings(const std::string& name_space, bool is_write) : name_space_(name_space), is_write_(is_write)
{
    nvs_open(name_space.c_str(), is_write_ ? NVS_READWRITE : NVS_READONLY, &nvs_handle_);
}


Settings::~Settings()
{
    if (nvs_handle_ != 0) {
        if (is_write_ && write_flag_) {
            ESP_ERROR_CHECK(nvs_commit(nvs_handle_));
        }
        nvs_close(nvs_handle_);
    }
}


std::string Settings::get_string(const std::string& key, const std::string& default_value)
{
    size_t length = 0;
    if (nvs_handle_ == 0 || nvs_get_str(nvs_handle_, key.c_str(), nullptr, &length) != ESP_OK) {
        return default_value;
    }

    std::string value;
    value.resize(length);
    ESP_ERROR_CHECK(nvs_get_str(nvs_handle_, key.c_str(), value.data(), &length));
    while (!value.empty() && value.back() == '\0') {
        value.pop_back();
    }
    return value;
}


void Settings::set_string(const std::string& key, const std::string& value) {
    if (is_write_ && nvs_handle_ != 0) {
        if (key.length() > 15) {
            ESP_LOGE("NVS", "Key '%s' too long (max 15 chars)", key.c_str());
            return;
        }
        ESP_ERROR_CHECK(nvs_set_str(nvs_handle_, key.c_str(), value.c_str()));
        write_flag_ = true;
    }
}


int32_t Settings::get_int(const std::string& key, int32_t default_value) {
    int32_t value;
    if (nvs_handle_ == 0 || nvs_get_i32(nvs_handle_, key.c_str(), &value) != ESP_OK) {
        return default_value;
    }
    return value;
}


void Settings::set_int(const std::string& key, int32_t value) {
    if (key.length() > 15) {
        ESP_LOGE("NVS", "Key '%s' too long (max 15 chars)", key.c_str());
        return;
    }
    if (is_write_ && nvs_handle_ != 0) {
        ESP_ERROR_CHECK(nvs_set_i32(nvs_handle_, key.c_str(), value));
        write_flag_ = true;
    }
}
