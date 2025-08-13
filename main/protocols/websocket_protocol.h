#pragma once
#include "protocol_adapter.h"
#include <string>
#include "board.h"

class WebsocketProtocol: public ProtocolAdapter
{
private:
    /* data */
public:
    WebsocketProtocol(/* args */);
    ~WebsocketProtocol();
    bool open_server_channel() override;
};