#pragma once


class ProtocolAdapter
{
private:
    /* data */
public:

    ProtocolAdapter(/* args */);
    ~ProtocolAdapter();
    virtual bool open_server_channel() = 0;
};


