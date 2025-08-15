#include "protocol_adapter.h"


ProtocolAdapter::ProtocolAdapter() {
}

ProtocolAdapter::~ProtocolAdapter() {
}

void WebsocketProtocol::close_server_channel() {
    if (websocket_ != nullptr) {
        delete websocket_;
        websocket_ = nullptr;
    }
}

bool WebsocketProtocol::is_open_server_channel() {
    return websocket_ != nullptr && websocket_->IsConnected();
}

bool ProtocolAdapter::send_detect_text(std::string text)
{
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return false;
    }

    cJSON_AddStringToObject(root, "type", "listen");
    cJSON_AddStringToObject(root, "state", "detect");
    cJSON_AddStringToObject(root, "text", text.c_str());
    cJSON_AddStringToObject(root, "session_id", session_id.c_str());

    char *json_str = cJSON_PrintUnformatted(root);
    if (!json_str) {
        cJSON_Delete(root);
        return false;
    }

    bool success = send_text(json_str);
    cJSON_free(json_str);
    cJSON_Delete(root);
    return success;
}

bool ProtocolAdapter::send_listening_start(bool is_auto) {
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        return false;
    }

    cJSON_AddStringToObject(root, "type", "listen");
    cJSON_AddStringToObject(root, "state", "start");
    cJSON_AddStringToObject(root, "mode", is_auto ? "auto" : "manual");
    cJSON_AddStringToObject(root, "session_id", session_id_.c_str());

    char *json_str = cJSON_PrintUnformatted(root);

    if (!json_str)
    {
        cJSON_Delete(root);
        return false;
    }

    bool success = send_text(json_str);
    free(json_str);
    cJSON_Delete(root);

    return success;
}

bool ProtocolAdapter::send_listening_stop() {
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        return false;
    }

    cJSON_AddStringToObject(root, "type", "listen");
    cJSON_AddStringToObject(root, "state", "stop");
    cJSON_AddStringToObject(root, "session_id", session_id_.c_str());

    char *json_str = cJSON_PrintUnformatted(root);

    if (!json_str)
    {
        cJSON_Delete(root);
        return false;
    }

    bool success = send_text(json_str);
    cJSON_free(json_str);
    cJSON_Delete(root);

    return success;
}