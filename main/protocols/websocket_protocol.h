#pragma once
#include "protocol_adapter.h"
#include <web_socket.h>
#include <string>
#include <cJSON.h>
#include <esp_event.h>


#define HELLO_TEXT "hello"


class WebsocketProtocol: public ProtocolAdapter
{
private:
    EventGroupHandle_t event_group_ = nullptr;
    bool send_hello_text();
    WebSocket* websocket_ = nullptr;
    void parse_text_route(const char* text);
    bool parse_hello_text(const char* type, const cJSON* root);

public:

    WebsocketProtocol(/* args */);
    ~WebsocketProtocol();
    bool open_server_channel() override;
    bool send_text(const std::string& text) override;
    void send_audio(const std::vector<uint8_t>& data) override;

};