#include "websocket_protocol.h"


WebsocketProtocol::WebsocketProtocol(/* args */)
{
}


WebsocketProtocol::~WebsocketProtocol()
{
}


bool WebsocketProtocol::open_server_channel()
{
    auto websocket_ = Board::GetInstance().GetWebSocket();

    std::string url = "ws://192.168.10.25:8000/";

    if (websocket_->Connect(url.c_str())) {
        return false;
    }

    return true;
}
