#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"


#include "ble_lib.h"

#define TAG "BLE_LIB"

// 定义广播数据（根据需要修改）
static uint8_t raw_adv_data[] = {
    0x02, 0x01, 0x06,  // flags
    0x02, 0x0a, 0xeb,  // tx power level
    0x03, 0x03, 0xFF, 0x00  // 16-bit UUID
};

// 定义扫描响应数据（根据需要修改）
static uint8_t raw_scan_rsp_data[] = {
    // ESP32S3_AI
    0x0f, 0x09, 'E', 'S', 'P', '3', '2', 'S', '3', '_', 'D', 'E', 'V', 'I', 'C', 'E'
};

// 声明回调函数
void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);



//TODO:Implement the BLE initialization function
esp_err_t ble_init(ble_write_callback_t callback, uint8_t* _ble_dev_name) {

    // esp_err_t ret;

    // // 释放之前可能存在的蓝牙配置
    // if (btStart() && esp_bluedroid_init() == ESP_OK) {
    //     esp_bluedroid_disable();
    //     esp_bluedroid_deinit();
    // }
    // btStop();

    // // 初始化NVS - 用于存储BLE配置
    // ret = nvs_flash_init();
    // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    //     ESP_ERROR_CHECK(nvs_flash_erase());
    //     ret = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK(ret);

    // // 初始化BLE控制器
    // esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    // ret = esp_bt_controller_init(&bt_cfg);
    // if (ret) {
    //     ESP_LOGE(TAG, "BLE controller init failed: %s", esp_err_to_name(ret));
    //     return ret;
    // }

    // // 使能BLE控制器
    // ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    // if (ret) {
    //     ESP_LOGE(TAG, "Enable BLE controller failed: %s", esp_err_to_name(ret));
    //     return ret;
    // }

    // // 初始化蓝牙协议栈
    // ret = esp_bluedroid_init();
    // if (ret) {
    //     ESP_LOGE(TAG, "Init bluetooth failed: %s", esp_err_to_name(ret));
    //     return ret;
    // }

    // // 使能蓝牙协议栈
    // ret = esp_bluedroid_enable();
    // if (ret) {
    //     ESP_LOGE(TAG, "Enable bluetooth failed: %s", esp_err_to_name(ret));
    //     return ret;
    // }

    // // 设置BLE设备名称
    // if (_ble_dev_name != NULL) {
    //     esp_ble_gap_set_device_name((const char*)_ble_dev_name);
    // } else {
    //     esp_ble_gap_set_device_name("ESP32S3_BLE");
    // }

    // // 设置GAP参数
    // esp_ble_gap_config_adv_data_raw(raw_adv_data, sizeof(raw_adv_data));
    // esp_ble_gap_config_scan_rsp_data_raw(raw_scan_rsp_data, sizeof(raw_scan_rsp_data));

    // // 注册回调函数
    // esp_ble_gap_register_callback(gap_event_handler);
    // esp_ble_gattc_register_callback(gattc_event_handler);

    return ESP_OK;
}


esp_err_t ble_close(void)
{
    return ESP_OK;
}


void submit_param_status_to_ble(uint8_t *msg, uint16_t len)
{

}
