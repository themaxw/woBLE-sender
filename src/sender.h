#ifndef SENDER_H
#define SENDER_H

#include <esp_now.h>
#include <WiFi.h>
#include <Message.h>



class Sender
{
private:
    /* data */
    esp_now_peer_info_t baseStationPeerInfo;
    uint8_t baseStationAddress[6] = {0x4C, 0x75, 0x25, 0x37, 0x2D, 0xB1};

public:
    Sender(/* args */);
    ~Sender();
    void setup();
    bool send_message(Message& message);

};




#endif //SENDER_H