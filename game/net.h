#ifndef NET_H
#define NET_H
#include"../thirdParty/enet/include/enet/enet.h"
#include<thread>

class Server
{
public:
	Server();
	Server(int port);
	bool getUseful();
private:
	ENetAddress address;
	ENetHost* server;
	bool useful = false;
};
class Client
{
};

#endif