#pragma once
#include <string_view>
#include <cstdint>


// 定义P3音效资源
namespace P3SoundLable
{
    extern const char p3_0_start[] asm("_binary_0_p3_start");
    extern const char p3_0_end[] asm("_binary_0_p3_end");
    static const std::string_view P3_0 {
        static_cast<const char*>(p3_0_start),
        static_cast<size_t>(p3_0_end - p3_0_start)
    };

    extern const char p3_1_start[] asm("_binary_1_p3_start");
    extern const char p3_1_end[] asm("_binary_1_p3_end");
    static const std::string_view P3_1 {
        static_cast<const char*>(p3_1_start),
        static_cast<size_t>(p3_1_end - p3_1_start)
    };

    extern const char p3_2_start[] asm("_binary_2_p3_start");
    extern const char p3_2_end[] asm("_binary_2_p3_end");
    static const std::string_view P3_2 {
        static_cast<const char*>(p3_2_start),
        static_cast<size_t>(p3_2_end - p3_2_start)
    };

    extern const char p3_3_start[] asm("_binary_3_p3_start");
    extern const char p3_3_end[] asm("_binary_3_p3_end");
    static const std::string_view P3_3 {
        static_cast<const char*>(p3_3_start),
        static_cast<size_t>(p3_3_end - p3_3_start)
    };

    extern const char p3_4_start[] asm("_binary_4_p3_start");
    extern const char p3_4_end[] asm("_binary_4_p3_end");
    static const std::string_view P3_4 {
        static_cast<const char*>(p3_4_start),
        static_cast<size_t>(p3_4_end - p3_4_start)
    };

    extern const char p3_5_start[] asm("_binary_5_p3_start");
    extern const char p3_5_end[] asm("_binary_5_p3_end");
    static const std::string_view P3_5 {
        static_cast<const char*>(p3_5_start),
        static_cast<size_t>(p3_5_end - p3_5_start)
    };

    extern const char p3_6_start[] asm("_binary_6_p3_start");
    extern const char p3_6_end[] asm("_binary_6_p3_end");
    static const std::string_view P3_6 {
        static_cast<const char*>(p3_6_start),
        static_cast<size_t>(p3_6_end - p3_6_start)
    };

    extern const char p3_7_start[] asm("_binary_7_p3_start");
    extern const char p3_7_end[] asm("_binary_7_p3_end");
    static const std::string_view P3_7 {
        static_cast<const char*>(p3_7_start),
        static_cast<size_t>(p3_7_end - p3_7_start)
    };

    extern const char p3_8_start[] asm("_binary_8_p3_start");
    extern const char p3_8_end[] asm("_binary_8_p3_end");
    static const std::string_view P3_8 {
        static_cast<const char*>(p3_8_start),
        static_cast<size_t>(p3_8_end - p3_8_start)
    };

    extern const char p3_9_start[] asm("_binary_9_p3_start");
    extern const char p3_9_end[] asm("_binary_9_p3_end");
    static const std::string_view P3_9 {
        static_cast<const char*>(p3_9_start),
        static_cast<size_t>(p3_9_end - p3_9_start)
    };

    extern const char p3_enter_ble_wifi_start[] asm("_binary_enter_ble_wifi_p3_start");
    extern const char p3_enter_ble_wifi_end[] asm("_binary_enter_ble_wifi_p3_end");
    static const std::string_view P3_ENTER_BLE_WIFI {
        static_cast<const char*>(p3_enter_ble_wifi_start),
        static_cast<size_t>(p3_enter_ble_wifi_end - p3_enter_ble_wifi_start)
    };

    extern const char p3_key_wake_up_start[] asm("_binary_key_wake_up_p3_start");
    extern const char p3_key_wake_up_end[] asm("_binary_key_wake_up_p3_end");
    static const std::string_view P3_KEY_WAKE_UP {
        static_cast<const char*>(p3_key_wake_up_start),
        static_cast<size_t>(p3_key_wake_up_end - p3_key_wake_up_start)
    };

    extern const char p3_success_start[] asm("_binary_success_p3_start");
    extern const char p3_success_end[] asm("_binary_success_p3_end");
    static const std::string_view P3_SUCCESS {
        static_cast<const char*>(p3_success_start),
        static_cast<size_t>(p3_success_end - p3_success_start)
    };

    extern const char p3_wifi_config_ok_start[] asm("_binary_wifi_config_ok_p3_start");
    extern const char p3_wifi_config_ok_end[] asm("_binary_wifi_config_ok_p3_end");
    static const std::string_view P3_WIFI_CONFIG_OK {
        static_cast<const char*>(p3_wifi_config_ok_start),
        static_cast<size_t>(p3_wifi_config_ok_end - p3_wifi_config_ok_start)
    };

    
    // 新增：数字音频标签数组（按索引 0~9 存储）
    static const std::string_view DIGIT_AUDIO[] = {
        P3_0, P3_1, P3_2, P3_3, P3_4,
        P3_5, P3_6, P3_7, P3_8, P3_9
    };

} 

struct P3HeaderStructure
{
    uint8_t type;
    uint8_t reserved;
    uint16_t payload_size;
    uint8_t payload[];
} __attribute__((packed));
