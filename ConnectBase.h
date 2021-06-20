// ConnectBase.h
#ifndef _CONNECT_BASE_H
#define _CONNECT_BASE_H
// #include "game.h"
// #include "GameMessage.h"
#include <WinSock2.h>
#include <process.h>
#include <iostream>

//#include "package.h"
class Package;
using std::vector;
using std::string;
#define rec_server (-1)

namespace game_connect{
    // const int message_fetched=0;
    // const int no_message=1;
    // const int time_out=2;

    // const int success=0;
    // const int error=-1;

    // const int game_on=16;
    // const int game_over=17;
    // const int game_give_me_card=18;
    // const int game_send_you_card=19;
    // const int game_quit=20;

    // enum CONNECT_STATE{
    //     MESSAGE_FETCHED,NO_MESSAGE,MESSAGE_SENT,MESSAGE_CANCELED,
    //     TIME_OUT,
    //     CONN_SUCCESS,CONN_ERROR,CONN_DENIED,CONN_QUIT,
    //     GAME_OVER,GAME_OPEN,GAME_ON,
    //     GAME_GIVE_ME_CARD,GAME_SEND_YOU_CARD,GAME_QUIT
    // };
    // void ErrorHandling(const char * message);
}

class ConnectBase{
    // private:
    // protected:
    //     static const int BUF_SIZE=1024;
    // public:
    //     static void ErrorHandling(const char* message);
    //     static int read_msg(SOCKET s,GameMessage& msg);
    //     static char* read_buf(SOCKET s,void* dst,char* buf,char* cur_read,int buf_size,int max_size);
};

#endif