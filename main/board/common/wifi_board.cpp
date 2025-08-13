#include "wifi_board.h"
 

#define WIFI_CONNECTED_BIT  (1 << 0)
#define BLE_CONFIG_BIT      (1 << 1)


static void send_param_to_ble(uint8_t *ssid, uint8_t *websocket_url, uint8_t *ip, uint16_t websocket_port)
{
    uint8_t msg[200];
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "code", 1);
    cJSON_AddStringToObject(root, "ip", (const char *)ip);
    cJSON_AddStringToObject(root, "ssid", (const char *)ssid);
    cJSON_AddStringToObject(root, "websocket_url", (const char *)websocket_url);
    cJSON_AddNumberToObject(root, "websocket_port", websocket_port);
    memset(msg, 0, sizeof(msg));
    char *json_str = cJSON_PrintUnformatted(root);
    if (json_str != NULL)
    {
        strncpy((char *)msg, json_str, sizeof(msg) - 1);
    }
    cJSON_free(json_str);
    cJSON_Delete(root);
    ESP_LOGI(TAG, "上报数据:%s", msg);
    submit_param_status_to_ble(msg, strlen((const char *)msg));
}


void wifi_event_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    // 处理WiFi事件
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                break; 
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI("WIFI", "已成功连接到 AP");
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                esp_wifi_connect();
                break;           
            default:
                break;
        }
    }
    // 处理IP事件
    uint8_t ip_addr[50];
    if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
            case IP_EVENT_STA_GOT_IP:
                ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
                memset(ip_addr, 0, sizeof(ip_addr));
                sprintf((char *)ip_addr, IPSTR, IP2STR(&event->ip_info.ip));
                ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
                char ssid[33] = {0};
                wifi_config_t wifi_config;
                esp_wifi_get_config(WIFI_IF_STA, &wifi_config);
                memcpy(ssid, wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid));
                send_param_to_ble((uint8_t *)ssid,(uint8_t *)"ws://",(uint8_t *)ip_addr,8000);

                Settings setting = Settings(SettingNameSpace::WIFI, true);
                setting.set_int(SettingKey::IS_ENTER_WIFI_CONFIG, 0);

                EventGroupHandle_t event_group_ = (EventGroupHandle_t)event_handler_arg;
                xEventGroupSetBits(event_group_, BLE_CONFIG_BIT);
                xEventGroupSetBits(event_group_, WIFI_CONNECTED_BIT);

            break;
        }
    }

}

void ble_recv_cb(const char *json_data, uint16_t length) {

    cJSON *root = cJSON_Parse((char *)json_data);
    if (root == NULL) {
        ESP_LOGE(TAG, "json data:%s", json_data);
        return;
    }

    if (!cJSON_HasObjectItem(root, "code")) {
        return;
    }
    if (!cJSON_HasObjectItem(root, "ssid")) {
        return;
    }
    if (!cJSON_HasObjectItem(root, "password")) {
        return;
    }

    int code = cJSON_GetObjectItem(root, "code")->valueint;

    switch (code) {
    case 1: 
        if (!cJSON_HasObjectItem(root, "ssid")) {
            return;
        }
        if (!cJSON_HasObjectItem(root, "password")) {
            return;
        }

        ESP_LOGI(TAG, "接收wifi配网消息 :%s", json_data);
        esp_netif_ip_info_t ip_info;
        esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
        if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
            // ip地址不为0表示已连接
            if (ip_info.ip.addr != 0) {
                char ip_addr[16] = {0};
                wifi_config_t wifi_config;
                esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

                esp_ip4addr_ntoa(&ip_info.ip, ip_addr, sizeof(ip_addr));
                // wifi名称也一样直接返回
                if (strcmp(cJSON_GetObjectItem(root, "ssid")->valuestring, (const char *)wifi_config.sta.ssid) == 0) {
                    send_param_to_ble((uint8_t *)wifi_config.sta.ssid, (uint8_t *)"ws://", (uint8_t *)ip_addr, 8000);
                    return;
                }
            }
        }

        wifi_config_t wifi_cfg = {
            .sta = {
                .threshold = {
                    .authmode = WIFI_AUTH_WPA2_PSK,
                },
                .pmf_cfg = {
                    .capable = true,
                    .required = false,
                }
            }
        };

        memset(wifi_cfg.sta.ssid, 0, sizeof(wifi_cfg.sta.ssid));
        memcpy(wifi_cfg.sta.ssid, cJSON_GetObjectItem(root, "ssid")->valuestring, strlen(cJSON_GetObjectItem(root, "ssid")->valuestring));

        memset(wifi_cfg.sta.password, 0, sizeof(wifi_cfg.sta.password));
        memcpy(wifi_cfg.sta.password, cJSON_GetObjectItem(root, "password")->valuestring, strlen(cJSON_GetObjectItem(root, "password")->valuestring));

        ESP_LOGI(TAG, "ssid :%s", wifi_cfg.sta.ssid);
        ESP_LOGI(TAG, "password :%s", wifi_cfg.sta.password);

        esp_wifi_stop();
        esp_wifi_disconnect();

        esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
        esp_wifi_start();

        break;
    }

    cJSON_Delete(root);
}

/*
* @brief WiFi配置类的构造函数
* 初始化WiFi相关的配置和事件处理程序
*/
WifiBoard::WifiBoard()
{
    event_group_ = xEventGroupCreate();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    // 初始化WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    // 注册WiFi事件处理程序
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, (void*)event_group_);
    // 注册IP事件处理程序                                                  
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, (void*)event_group_);
    // 设置WiFi工作模式为STA模式
    esp_wifi_set_mode(WIFI_MODE_STA);
}


WifiBoard:: ~WifiBoard()
{
    vEventGroupDelete(event_group_);
}


void WifiBoard::start_net()
{
    bool is_enter_wifi_config = false;
    // 获取nvs参数判断是否要进入配网模式
    Settings seting = Settings(SettingNameSpace::WIFI, false);
    int flag = seting.get_int(SettingKey::IS_ENTER_WIFI_CONFIG, -1);

    flag == 0 ? is_enter_wifi_config = false : is_enter_wifi_config = true;

    while (true)
    {
        if (is_enter_wifi_config)
        {
            ESP_LOGI(TAG, "start enter_wifi_config_mode");
            enter_wifi_config_mode();
            auto bits = xEventGroupWaitBits(event_group_, BLE_CONFIG_BIT, pdTRUE, pdFALSE, pdMS_TO_TICKS(90000));
            if (bits & BLE_CONFIG_BIT)
            {
                ESP_LOGI(TAG, "wifi connected");
                ble_close();
                return;
            }
        }

        ESP_LOGI(TAG, "start connect wifi");
        esp_wifi_start();
        auto bits = xEventGroupWaitBits(event_group_, WIFI_CONNECTED_BIT, pdTRUE, pdFALSE, pdMS_TO_TICKS(30000));
        if (bits & WIFI_CONNECTED_BIT)
        {
            ESP_LOGI(TAG, "wifi connected");
            return;
        }
        // 再次进入配网模式
        is_enter_wifi_config = true;
    }
}


void WifiBoard::stop_net()
{
    esp_wifi_stop();
}


void WifiBoard::enter_wifi_config_mode()
{
    ble_init(ble_recv_cb, (uint8_t *)"ESP32S3_AI");
}


WebSocket* WifiBoard::GetWebSocket() {
    
    return new WebSocket(new TcpTransport());
}