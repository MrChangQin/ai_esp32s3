#include "websocket_protocol.h"
#include "protocol_adapter.h"
#include "cJSON.h"
#include "app_config.h"
#include <cstring>
#include <esp_log.h>


#define TAG "WebsocketProtocol"
#define RECV_SERVER_HELLO_EVENT_ID (1 << 0)


WebsocketProtocol::WebsocketProtocol(/* args */) {
    websocket_ = Board::GetInstance().GetWebSocket();
    event_group_ = xEventGroupCreate();
}

WebsocketProtocol::~WebsocketProtocol() {
    vEventGroupDelete(event_group_);
}

void WebsocketProtocol::send_audio(const std::vector<uint8_t> &data) {
    if (websocket_ == nullptr) {
        return;
    }
    websocket_->Send(data.data(), data.size(), true);
}


bool WebsocketProtocol::send_hello_text() {
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return false;
    }

    cJSON_AddStringToObject(root, "type", "hello");
    cJSON_AddNumberToObject(root, "version", VERSION);
    cJSON_AddStringToObject(root, "transport", "websocket");

    cJSON *audio_obj = cJSON_CreateObject();
    if (audio_obj == NULL) {
        cJSON_Delete(root);
        return false;
    }

    cJSON_AddStringToObject(audio_obj, "format", "opus");
    cJSON_AddNumberToObject(audio_obj, "sample_rate", OPUS_SAMPLE_RATE);
    cJSON_AddNumberToObject(audio_obj, "channels", 1);
    cJSON_AddNumberToObject(audio_obj, "frame_duration", OPUS_FRAME_DURATION);
    cJSON_AddItemToObject(root, "audio_params", audio_obj);

    char *json_str = cJSON_PrintUnformatted(root);
    if (json_str == NULL) {
        cJSON_Delete(root);
        return false;
    }

    bool success = send_text(json_str);
    cJSON_free(json_str);
    cJSON_Delete(root);

    return success;
}

bool WebsocketProtocol::send_text(const std::string &text) {
    if (websocket_ == nullptr) {
        return false;
    }
    return websocket_->Send(text.c_str());
}

bool WebsocketProtocol::parse_hello_text(const char *type, const cJSON *root)
{
    if (strcmp(type, HELLO_TEXT) != 0) {
        return false;
    }

    cJSON *session_id_obj = cJSON_GetObjectItem(root, "session_id");
    if (session_id_obj == NULL) {
        return false;
    }
    session_id_ = session_id_obj->valuestring;

    cJSON *audio_params_obj = cJSON_GetObjectItem(root, "audio_params");
    if (audio_params_obj == NULL) {
        return false;
    }

    cJSON *sample_rate_obj = cJSON_GetObjectItem(audio_params_obj, "sample_rate");
    if (sample_rate_obj == NULL) {
        return false;
    }
    sample_rate_ = sample_rate_obj->valueint;

    cJSON *frame_duration_obj = cJSON_GetObjectItem(audio_params_obj, "frame_duration");
    if (frame_duration_obj == NULL) {
        return false;
    }
    frame_duration_ms_ = frame_duration_obj->valueint;

    cJSON *channels_obj = cJSON_GetObjectItem(audio_params_obj, "channels");
    if (channels_obj == NULL) {
        return false;
    }
    channels_ = channels_obj->valueint;

    xEventGroupSetBits(event_group_, RECV_SERVER_HELLO_EVENT_ID);

    return true;
}

void WebsocketProtocol::parse_text_route(const char *text) {
    cJSON *root = cJSON_Parse(text);
    if (root == NULL) {
        return;
    }

    cJSON *typeObj = cJSON_GetObjectItem(root, "type");
    if (typeObj == NULL) {
        ESP_LOGI(TAG, "type is NULL");
        cJSON_Delete(root);
        return;
    }

    if (parse_hello_text(typeObj->valuestring, root)) {
        cJSON_Delete(root);
        return;
    }
}

bool WebsocketProtocol::open_server_channel() {

    std::string url = CONFIG_WEBSOCKET_URL;

    websocket_->OnData([this](const char *data, size_t len, bool binary) {
        if (binary) { 
            if (audio_msg_callback_) {
                std::vector<uint8_t> buffer(data, data + len);
                audio_msg_callback_(std::move(buffer));
            }
        } else {
            ESP_LOGI(TAG, "Received binary message");
            parse_text_route(data);
        }
    });

    if (!websocket_->Connect(url.c_str())) {
        return false;
    }

    uint32_t bits = 0;
    for (size_t i = 0; i < 3; i++) {
        send_hello_text();
        bits = xEventGroupWaitBits(event_group_, RECV_SERVER_HELLO_EVENT_ID, false, true, pdMS_TO_TICKS(5000));
        if (bits & RECV_SERVER_HELLO_EVENT_ID) {
            break;
        }
    }

    if (bits & RECV_SERVER_HELLO_EVENT_ID) {
        
        if (server_set_param_callback_) {
            server_set_param_callback_();
        }
        return true;
    }

    ESP_LOGI(TAG, "server hello timeout");
    return false;
}

