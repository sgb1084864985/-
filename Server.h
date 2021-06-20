//Server.h
#ifndef _SERVER_H
#define _SERVER_H
#include "ConnectBase.h"
// #include "GameMessage.h"
#include "Port.h"
#include <queue>

// class Package;
// using namespace game_connect;

class Server:public Port{
    private:
    //...

    public:
    void SendGameMsg(const Package &p) const;
    Package CollectGameMsg(int sender=rec_server);

    Server();
    ~Server();
    int OpenRoom(int game_type);  
    int StartGame();

};
#endif