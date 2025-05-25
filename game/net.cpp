#include"net.h"
#include<iostream>
#include<syncstream>
#include<algorithm>
#include"../wrapper/Log.h"

Server::Server()
{
	address.host = ENET_HOST_ANY;
	for (int port = 2000; port <= 3000; port++)
	{
		address.port = port;
		server = enet_host_create(&address, 32, 1, 0, 0);
		if (server)
		{

			logcout << "[INFO/SERVER]: Successfully established a server on port " << port << std::endl;
			state = true;
			mThread = std::thread(&Server::run, this);
			return;
		}
	}
	logcout << "[WARN/SERVER]: Failed to establish a server!" << std::endl;
}
Server::Server(int port)
{
	address.host = ENET_HOST_ANY;
	address.port = port;
	server = enet_host_create(&address, 64, 1, 0, 0);
	if (server)
	{

		logcout << "[INFO/SERVER]: Successfully established a server on port " << port << std::endl;
		state = true;
		mThread = std::thread(&Server::run, this);
		return;
	}
	logcout << "[WARN/SERVER]: Failed to establish a server on port " << port << " !" << std::endl;
}
Server::~Server()
{
	state = false;
	if (mThread.joinable())
		mThread.join();
	if (server != nullptr)
	{
		enet_host_destroy(server);
		server = nullptr;
	}
}
bool Server::getState()
{
	return state;
}
void Server::run()
{
	clock = std::chrono::system_clock::now();
	ENetEvent event;
	while (state)
	{
		if (clock + std::chrono::milliseconds(50) < std::chrono::system_clock::now())
		{
			clock = std::chrono::system_clock::now();
			{
				std::lock_guard<std::mutex> lock(clientsMutex);
				for (int i = clients.size() - 1; i >= 0; i--)
				{
					if (clients[i].lastHeartBeatTime + std::chrono::seconds(5) < std::chrono::system_clock::now())
					{
						logcout << "[INFO/CLIENT]: Interrupted the connection because of no response from client " << clients[i].peer->address.host << ":" << clients[i].peer->address.port << std::endl;
						if (clients[i].peer != nullptr)
							enet_peer_disconnect_now(clients[i].peer, 0);
						clients.erase(clients.begin() + i);
						Server::sentAllMemberList();
					}
				}
				for (auto& client : clients)
				{
					std::string sentmsg = "Heart";
					ENetPacket* packet = enet_packet_create(sentmsg.c_str(), sentmsg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(client.peer, 0, packet);
				}
			}
		}
		if (enet_host_service(server, &event, 10) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				if (!isGaming)
					logcout << "[INFO/SERVER]: Client connected from " << event.peer->address.host << ":" << event.peer->address.port << std::endl;
				else
					enet_peer_disconnect_now(event.peer, 0);
				break;
			case  ENET_EVENT_TYPE_RECEIVE:
			{
				std::string getmsg(reinterpret_cast<const char*>(event.packet->data));
				if (getmsg == "Shake hands")
				{
					std::string rspmsg = "Shake hands";
					ENetPacket* packet = enet_packet_create(rspmsg.c_str(), rspmsg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(event.peer, 0, packet);
					enet_host_flush(server);
				}
				if (getmsg.substr(0, 16) == "Shake both hands")
				{
					logcout << "[INFO/SERVER]: Successfully get shake hands response from client " << event.peer->address.host << ":" << event.peer->address.port << std::endl;
					std::lock_guard<std::mutex> lock(clientsMutex);
					clients.push_back({ event.peer,getmsg.substr(17,getmsg.size() - 17),std::chrono::system_clock::now() });
					Server::sentAllMemberList();
				}
				if (getmsg == "Beat")
				{
					std::lock_guard<std::mutex> lock(clientsMutex);
					for (auto& client : clients)
						if (client.peer == event.peer)
							client.lastHeartBeatTime = std::chrono::system_clock::now();
				}
				if (getmsg.substr(0, 4) == "IBC:")
				{
					std::lock_guard<std::mutex> lock(clientsMutex);
					for (auto& client : clients)
						if (client.peer == event.peer)
							client.hexaColors = getmsg.substr(4, getmsg.size() - 4);
				}
				if (getmsg.substr(0, 4) == "IBT:")
				{
					std::lock_guard<std::mutex> lock(clientsMutex);
					for (auto& client : clients)
						if (client.peer == event.peer)
							client.time = std::stof(getmsg.substr(4, getmsg.size() - 4));
				}
				if (getmsg == "HexaMap Finish")
				{
					std::lock_guard<std::mutex> lock(clientsMutex);
					for (auto& client : clients)
						if (client.peer == event.peer)
							client.finish = true;
				}

				enet_packet_destroy(event.packet);
			}
			break;
			case  ENET_EVENT_TYPE_DISCONNECT:
			{
				std::lock_guard<std::mutex> lock(clientsMutex);
				for (int i = 0; i < clients.size(); i++)
					if (clients[i].peer == event.peer)
					{
						logcout << "[INFO/CLIENT]: The connection was interrupted by client " << clients[i].peer->address.host << ":" << clients[i].peer->address.port << " !" << std::endl;
						clients.erase(clients.begin() + i);
						Server::sentAllMemberList();
						break;
					}
			}
			break;
			}
		}
		if (isGaming)
		{
			sentAllMemberInfo();
			bool allFinish = true;
			{
				std::lock_guard<std::mutex> lock(clientsMutex);
				for (auto& client : clients)
				{
					if (!client.finish)
					{
						allFinish = false;
					}
				}
			}
			if (allFinish)
				sentGameState(false);
		}
		enet_host_flush(server);
	}
	logcout << "[INFO/SERVER] Server off line." << std::endl;
	for (int i = server->peerCount - 1; i >= 0; i--)
	{
		ENetPeer* peer = &server->peers[i];
		enet_peer_disconnect_now(peer, 0);
	}
	if (server != nullptr)
	{
		enet_host_destroy(server);
		server = nullptr;
	}
}
std::string Server::getIP()
{
	std::string ipStr;
	char ip[32];
	enet_address_get_host_ip(&address, ip, sizeof(ip));
	ipStr = ip;
	return ipStr;
}
int Server::getPort()
{
	return address.port;
}
void Server::sentAllMemberList()
{
	std::string sentmsg = "AML:";
	{
		for (auto& client : clients)
			sentmsg += client.useName + "|";
		for (auto& client : clients)
		{
			ENetPacket* packet = enet_packet_create(sentmsg.c_str(), sentmsg.size(), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(client.peer, 0, packet);
		}
	}
}
void Server::sentAllMemberInfo()
{
	std::lock_guard<std::mutex> lock(clientsMutex);
	std::string sentmsg = "AMI:";
	{
		for (auto& client : clients)
			sentmsg += client.useName + "/" + std::to_string(client.time) + "/" + client.hexaColors + "|";
		for (auto& client : clients)
		{
			ENetPacket* packet = enet_packet_create(sentmsg.c_str(), sentmsg.size(), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(client.peer, 0, packet);
		}
	}
}
void Server::sentSandBoxInfo(bool rotation, bool colorChange, bool colorful, int levelBase)
{
	std::string sentmsg = "SBI:";
	sentmsg += std::to_string(rotation) + "|" + std::to_string(colorChange) + "|" + std::to_string(colorful) + "|" + std::to_string(levelBase);
	{
		std::lock_guard<std::mutex> lock(clientsMutex);
		for (auto& client : clients)
		{
			ENetPacket* packet = enet_packet_create(sentmsg.c_str(), sentmsg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(client.peer, 0, packet);
		}
	}
}
void Server::sentGameState(bool state)
{
	{
		std::lock_guard<std::mutex> lock(clientsMutex);
		for (auto& client : clients)
		{
			client.time = -1;
			client.hexaColors.clear();
			client.finish = false;
		}
	}

	std::string sentmsg = "GST:";
	if (state)
	{
		sentmsg += "T";
		isGaming = true;
	}
	else
	{
		sentmsg += "F";
		isGaming = false;
	}
	{
		std::lock_guard<std::mutex> lock(clientsMutex);
		for (auto& client : clients)
		{
			ENetPacket* packet = enet_packet_create(sentmsg.c_str(), sentmsg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(client.peer, 0, packet);
		}
	}
}
void Server::sentHexaMapInfo(int size, std::string colors)
{
	std::string sentmsg = "HMI:";
	sentmsg += std::to_string(size) + "|" + colors;
	{
		std::lock_guard<std::mutex> lock(clientsMutex);
		for (auto& client : clients)
		{
			ENetPacket* packet = enet_packet_create(sentmsg.c_str(), sentmsg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(client.peer, 0, packet);
		}
	}
}

Client::Client(std::array<int, 4> ip, int port, std::string name) : userName(name)
{
	ipStr = std::to_string(ip[0]) + "." + std::to_string(ip[1]) + "." + std::to_string(ip[2]) + "." + std::to_string(ip[3]);
	client = enet_host_create(NULL, 1, 1, 0, 0);

	enet_address_set_host(&serverAddr, ipStr.c_str());
	mPort = port;
	serverAddr.port = port;

	serverPeer = enet_host_connect(client, &serverAddr, 1, 0);

	ENetEvent event;
	if (enet_host_service(client, &event, 10) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {

		logcout << "[INFO/CLIENT]: Successfully connected to server " << ipStr << ":" << port << std::endl;
		state = true;
		mThread = std::thread(&Client::run, this);
	}
	else
	{

		logcout << "[WARN/CLIENT]: Failed to connect to server!" << std::endl;
		enet_peer_reset(serverPeer);
	}
}
Client::Client(std::string ip, int port, std::string name) : userName(name)
{
	ipStr = ip;
	client = enet_host_create(NULL, 1, 1, 0, 0);

	enet_address_set_host(&serverAddr, ipStr.c_str());
	mPort = port;
	serverAddr.port = port;

	serverPeer = enet_host_connect(client, &serverAddr, 1, 0);

	ENetEvent event;
	if (enet_host_service(client, &event, 10) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {

		logcout << "[INFO/CLIENT]: Successfully connected to server " << ipStr << ":" << port << std::endl;
		state = true;
		mThread = std::thread(&Client::run, this);
	}
	else {

		logcout << "[WARN/CLIENT]: Failed to connect to server!" << std::endl;
		enet_peer_reset(serverPeer);
	}
}
Client::~Client()
{
	state = false;
	if (mThread.joinable())
		mThread.join();
	if (serverPeer != nullptr)
	{
		enet_peer_disconnect_now(serverPeer, 0);
		serverPeer = nullptr;
	}
	if (client != nullptr)
		enet_host_destroy(client);
}
bool Client::getState()
{
	return state;
}
void Client::run()
{
	{
		std::string sentmsg = "Shake hands";
		ENetPacket* packet = enet_packet_create(sentmsg.c_str(), sentmsg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(serverPeer, 0, packet);
		enet_host_flush(client);
	}
	ENetEvent event;
	lastHeartBeatTime = std::chrono::system_clock::now();
	while (state)
	{
		if (enet_host_service(client, &event, 10) > 0)
		{
			if (lastHeartBeatTime + std::chrono::seconds(5) < std::chrono::system_clock::now())
			{
				logcout << "[INFO/CLIENT]: Interrupted the connection because of no response from server " << serverPeer->address.host << ":" << serverPeer->address.port << "(" << ipStr << ":" << mPort << ")" << std::endl;
				state = false;
				break;
			}

			switch (event.type)
			{
			case  ENET_EVENT_TYPE_RECEIVE:
			{
				std::string getmsg(reinterpret_cast<const char*>(event.packet->data));
				if (getmsg == "Shake hands")
				{
					std::string rspmsg = "Shake both hands";
					rspmsg += "|" + userName;
					ENetPacket* packet = enet_packet_create(rspmsg.c_str(), rspmsg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(event.peer, 0, packet);
					lastHeartBeatTime = std::chrono::system_clock::now();
					logcout << "[INFO/CLIENT]: Successfully get shake hands response from server " << event.peer->address.host << ":" << event.peer->address.port << "(" << ipStr << ":" << mPort << ")" << std::endl;
				}
				if (getmsg == "Heart")
				{
					std::string rspmsg = "Beat";
					ENetPacket* packet = enet_packet_create(rspmsg.c_str(), rspmsg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(event.peer, 0, packet);
					lastHeartBeatTime = std::chrono::system_clock::now();
				}
				if (getmsg.substr(0, 4) == "AML:")
				{
					AML.clear();
					std::string name;
					std::lock_guard<std::mutex> lock(AMLMutex);
					for (int i = 4; i < getmsg.size(); i++)
					{
						if (getmsg[i] != '|')
							name += getmsg[i];
						else
						{
							AML.push_back(name);
							name.clear();
						}
					}
				}
				if (getmsg.substr(0, 4) == "AMI:")
				{
					std::vector<ClientInfo> tempAMI;
					std::string name;
					std::string colors;
					std::string timeStr;
					int step = 0;
					for (int i = 4; i < getmsg.size(); i++)
					{
						switch (getmsg[i])
						{
						case'|':
							step = 0;
							if (!name.empty() && !timeStr.empty())
								tempAMI.push_back({ name,colors,std::stof(timeStr) });
							name.clear();
							colors.clear();
							timeStr.clear();
							break;
						case '/':
							step++;
							break;
						default:
							switch (step)
							{
							case 0:
								name += getmsg[i];
								break;
							case 1:
								timeStr += getmsg[i];
								break;
							case 2:
								colors += getmsg[i];
								break;
							}
							break;
						}
					}
					std::stable_sort(tempAMI.begin(), tempAMI.end(), [](const ClientInfo& a, const ClientInfo& b) {return a.time < b.time; });
					{
						std::lock_guard<std::mutex> lock(AMIMutex);
						AMI = std::move(tempAMI);
					}
				}
				if (getmsg.substr(0, 4) == "SBI:")
				{
					std::lock_guard<std::mutex> lock(SandBoxInfoMutex);
					if (getmsg.substr(4, 1) == "1")
						isRotation = true;
					else
						isRotation = false;
					if (getmsg.substr(6, 1) == "1")
						isColorChange = true;
					else
						isColorChange = false;
					if (getmsg.substr(8, 1) == "1")
						isColorful = true;
					else
						isColorful = false;
					std::string strLevelBase = getmsg.substr(10, getmsg.size() - 10);
					mLevelBase = std::stoi(strLevelBase);
				}
				if (getmsg.substr(0, 4) == "GST:")
				{
					if (getmsg.substr(4, 1) == "T")
					{
						isGaming = true;
						isHMIget = false;
						std::cout << "Gaming" << std::endl;
					}
					else
						isGaming = false;
				}
				if (getmsg.substr(0, 4) == "HMI:")
				{
					std::lock_guard<std::mutex> lock(HexaMapColorsMutex);
					hexaMapColors.clear();
					std::string numberStr;
					bool flag = false;
					for (int i = 4; i < getmsg.size(); i++)
					{
						if (getmsg[i] == '|')
						{
							flag = true;
							continue;
						}
						if (!flag)
							numberStr += getmsg[i];
						else
							hexaMapColors += getmsg[i];
					}
					mapSize = std::stoi(numberStr);
					isHMIget = true;
				}
				enet_packet_destroy(event.packet);
			}
			break;
			case ENET_EVENT_TYPE_DISCONNECT:
				state = false;
				logcout << "[INFO/CLIENT]: The connection was interrupted by server!" << std::endl;
			}
		}
		enet_host_flush(client);
	}
	logcout << "[INFO/CLIENT]: Client off line." << std::endl;
	if (serverPeer != nullptr)
	{
		enet_peer_disconnect_now(serverPeer, 0);
		serverPeer = nullptr;
	}
	if (client != nullptr)
	{
		enet_host_destroy(client);
		client = nullptr;
	}
}

std::string Client::getIP()
{
	return ipStr;
}
int Client::getPort()
{
	return mPort;
}
std::vector<std::string> Client::getMemberList()
{
	return AML;
}
void Client::getMemberInfo(int index, std::string& userName, float& time, std::string& colors)
{
	std::lock_guard<std::mutex> lock(AMIMutex);
	if (index >= AMI.size())
		return;
	userName = AMI[index].useName;
	time = AMI[index].time;
	colors = AMI[index].hexaColors;
}
void Client::getSandBoxInfo(bool& isR, bool& isCC, bool& isC, int& LB)
{
	std::lock_guard<std::mutex> lock(SandBoxInfoMutex);
	isR = isRotation;
	isCC = isColorChange;
	isC = isColorful;
	LB = mLevelBase;
}
bool Client::getGameState()
{
	return isGaming;
}
bool Client::getHMIState()
{
	return isHMIget;
}
void Client::getHexaMapInfo(int& mapSize, std::string& colors)
{
	mapSize = this->mapSize;
	colors = hexaMapColors;
}
void Client::sentMapInfoBack(float time, std::string colors)
{
	std::string sentmsg0 = "IBC:" + colors;
	std::string sentmsg1 = "IBT:" + std::to_string(time);
	ENetPacket* packet0 = enet_packet_create(sentmsg0.c_str(), sentmsg0.size() + 1, ENET_PACKET_FLAG_RELIABLE);
	ENetPacket* packet1 = enet_packet_create(sentmsg1.c_str(), sentmsg1.size() + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(serverPeer, 0, packet0);
	enet_peer_send(serverPeer, 0, packet1);
}
void Client::sentHexaMapFinish()
{
	std::string sentmsg = "HexaMap Finish";
	ENetPacket* packet = enet_packet_create(sentmsg.c_str(), sentmsg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(serverPeer, 0, packet);
}