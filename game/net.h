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
	std::string hexaColors;
	float time = 0;
	bool finish = false;
};
struct ClientInfo
{
	std::string useName;
	std::string hexaColors;
	float time = 0;
};
class Server
{
public:
	Server();
	Server(int port);
	~Server();
	bool getState();
	void run();
	std::string getIP();
	int getPort();
	void sentAllMemberList();
	void sentAllMemberInfo();
	void sentSandBoxInfo(bool rotation, bool colorChange, bool colorful, int levelBase);
	void sentGameState(bool state);
	void sentHexaMapInfo(int size, std::string colors);
private:
	std::atomic<bool> state;
	std::thread mThread;
	ENetAddress address;
	ENetHost* server;
	std::vector<ClientData> clients;
	std::mutex clientsMutex;
	std::chrono::time_point<std::chrono::system_clock> clock;
	std::atomic<bool> isGaming = false;
};

class Client
{
public:
	Client(std::array<int, 4> ip, int port, std::string userName);
	Client(std::string ip, int port, std::string userName);
	~Client();
	bool getState();
	void run();
	std::string getIP();
	int getPort();
	std::vector<std::string> getMemberList();
	void getMemberInfo(int index, std::string& userName, float& time, std::string& colors);
	void getSandBoxInfo(bool& isR, bool& isCC, bool& isC, int& LB);
	bool getGameState();
	bool getHMIState();
	void getHexaMapInfo(int& mapSize, std::string& colors);
	void sentMapInfoBack(float time, std::string colors);
	void sentHexaMapFinish();
private:
	std::atomic<bool> state;
	std::thread mThread;
	ENetHost* client;
	ENetAddress serverAddr;
	ENetPeer* serverPeer;
	std::string ipStr;
	int mPort;

	std::string userName;
	std::mutex AMLMutex;
	std::vector<std::string> AML;
	std::mutex AMIMutex;
	std::vector<ClientInfo> AMI;

	std::mutex SandBoxInfoMutex;
	std::atomic<bool> isRotation, isColorChange, isColorful;
	std::atomic<int> mLevelBase = 0;

	std::atomic<bool> isGaming = false;

	std::mutex HexaMapColorsMutex;
	std::atomic<bool> isHMIget = false;
	std::atomic<int> mapSize;
	std::string hexaMapColors;


	std::chrono::time_point<std::chrono::system_clock> lastHeartBeatTime;
};

#endif