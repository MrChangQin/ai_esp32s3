#pragma once
#include <string>
#include <nvs_flash.h>

namespace SettingNameSpace {
    constexpr const char* WIFI = "wifi";
}

namespace SettingKey {
    constexpr const char* IS_ENTER_WIFI_CONFIG = "wifi_config";
}

class Settings {
private:
    std::string name_space_;
    nvs_handle_t nvs_handle_ = 0;
    bool is_write_ = false;
    bool write_flag_ = false;

public:
    Settings(const std::string& name_space, bool is_write = false);
    std::string get_string(const std::string &key, const std::string &default_value);
    void set_string(const std::string &key, const std::string &value);
    int32_t get_int(const std::string &key, int32_t default_value);
    void set_int(const std::string& key, int32_t value);
    ~Settings();
};