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
#include<mutex>

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
	bool getState();
	void run();
	void sentAllMemberList();
	void sentSandBoxInfo(bool rotation, bool colorChange, bool colorful, int levelBase);
private:
	std::atomic<bool> state;
	std::thread mThread;
	ENetAddress address;
	ENetHost* server;
	std::vector<ClientData> clients;
	std::mutex clientsMutex;
	std::chrono::time_point<std::chrono::system_clock> clock;
};

class Client
{
public:
	Client(std::array<int, 4> ip, int port, std::string userName);
	~Client();
	bool getState();
	void run();
	std::vector<std::string> getMemberList();
	void getSandBoxInfo(bool &isR,bool&isCC,bool &isC,int &LB);
private:
	std::atomic<bool> state;
	std::thread mThread;
	ENetHost* client;
	ENetAddress serverAddr;
	ENetPeer* serverPeer;
	std::string ipStr;
	int mPort;

	std::string userName;
	std::vector<std::string> AML;

	std::mutex SandBoxInfoMutex;
	std::atomic<bool> isRotation, isColorChange, isColorful;
	std::atomic<int> mLevelBase = 0;

	std::chrono::time_point<std::chrono::system_clock> lastHeartBeatTime;
};

#endif