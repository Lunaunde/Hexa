#ifndef NET_H
#define NET_H
#include"../thirdParty/enet/include/enet/enet.h"
#include<thread>
#include<string>
#include<array>
#include<vector>
#include<atomic>
#include<chrono>
#include<ctime>

struct ClientData
{
	ENetPeer* peer;
	std::string useName;
	std::chrono::time_point<std::chrono::system_clock> lastHeartBeatTime;
};
class Server
{
public:
	Server();
	Server(int port);
	~Server();
	bool getUseful();
	void run();
private:
	std::atomic<bool> state;
	std::thread mThread;
	ENetAddress address;
	ENetHost* server;
	ENetEvent event;
	bool useful = false;
	std::vector<ClientData> clients;
	std::chrono::time_point<std::chrono::system_clock> clock;
};

class Client
{
public:
	Client(std::array<int, 4> ip, int port);
	~Client();
	void run();
private:
	std::atomic<bool> state;
	std::thread mThread;
	ENetHost* client;
	ENetAddress serverAddr;
	ENetPeer* serverPeer;
	std::string ipStr;
	int mPort;
	std::chrono::time_point<std::chrono::system_clock> lastHeartBeatTime;
};

#endif