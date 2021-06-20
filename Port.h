// Port.h
#ifndef _PORT_H
#define _PORT_H
#include "ConnectBase.h"
#include <queue>
// using namespace game_connect;
class Port:public ConnectBase{
    protected:
        // CONNECT_STATE state;
        // SOCKADDR_IN addr;
        // static int read_msg(SOCKET s,GameMessage& msg);
        // static char* read_buf(SOCKET s,void* dst,char* buf,char* cur_read,int buf_size,int max_size);
        // static int MsgBufClear(queue<GameMessage>& buf);
        // CONNECT_STATE SendInfo(const GameMessage& msg, SOCKET clntsocket)const;

    public:
        virtual void SendGameMsg(const Package &p) const;
        virtual Package CollectGameMsg(int sender=rec_server);
};
#endif