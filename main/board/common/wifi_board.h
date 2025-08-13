#pragma once
#include "board.h"
#include <esp_netif.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_wifi_default.h>
#include <esp_err.h>
#include <esp_mac.h>
#include <esp_log.h>
#include <cstring>
#include <cJSON.h>
#include <ble_lib.h>
#include <tcp_transport.h>
#include <setting.h>


#define TAG "WifiBoard"

class WifiBoard: public Board
{
private:
    void enter_wifi_config_mode();
    EventGroupHandle_t event_group_ = nullptr;
    
public:
    WifiBoard();
    ~WifiBoard();
    void start_net() override;
    void stop_net() override;
    
    WebSocket* GetWebSocket() override;
};

