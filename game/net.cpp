#include"net.h"
#include<iostream>
#include <syncstream>
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
			useful = true;
			state = true;
			mThread = std::thread(&Server::run, this);
			mThread.detach();
			return;
		}
	}
	logcout << "[WARN/SERVER]: Failed to establish a server!" << std::endl;
}
Server::Server(int port)
{
	address.host = ENET_HOST_ANY;
	address.port = port;
	server = enet_host_create(&address, 32, 1, 0, 0);
	if (server)
	{

		logcout << "[INFO/SERVER]: Successfully established a server on port " << port << std::endl;
		useful = true;
		state = true;
		mThread = std::thread(&Server::run, this);
		mThread.detach();
		return;
	}
	logcout << "[WARN/SERVER]: Failed to establish a server on port " << port << " !" << std::endl;
}
Server::~Server()
{
	state = false;
	if (server != nullptr)
		enet_host_destroy(server);
}
bool Server::getUseful()
{
	return useful;
}
void Server::run()
{
	clock = std::chrono::system_clock::now();
	while (state)
	{
		if (clock + std::chrono::seconds(1) >= std::chrono::system_clock::now())
		{
			clock = std::chrono::system_clock::now();
			for (int i = clients.size() - 1; i >= 0; i--)
			{
				if (clients[i].lastHeartBeatTime + std::chrono::seconds(5) < std::chrono::system_clock::now())
				{
					logcout << "[INFO/SERVER]: Client " << clients[i].peer->address.host << ":" << clients[i].peer->address.port << " disconnected!" << std::endl;
					enet_peer_disconnect_now(clients[i].peer, 0);
					clients.erase(clients.begin() + i);
				}
			}
			for (auto& client : clients)
			{
				std::string sentmsg = "Heart";
				ENetPacket* packet = enet_packet_create(sentmsg.c_str(), sentmsg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(client.peer, 0, packet);
			}
		}
		if (enet_host_service(server, &event, 10) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				logcout << "[INFO/SERVER]: Client connected from " << event.peer->address.host << ":" << event.peer->address.port << std::endl;
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
				if (getmsg == "Shake both hands")
				{
					logcout << "[INFO/SERVER]: Successfully get shake hands response from client " << event.peer->address.host << ":" << event.peer->address.port << std::endl;
					clients.push_back({ event.peer,"",std::chrono::system_clock::now() });
				}
				if (getmsg == "Beat")
				{
					for (auto& client : clients)
						if (client.peer == event.peer)
							client.lastHeartBeatTime = std::chrono::system_clock::now();
				}
				enet_packet_destroy(event.packet);
			}
			break;
			}
		}
		enet_host_flush(server);
	}
	logcout << "[INFO/SERVER] Server off line." << std::endl;
	for (size_t i = server->peerCount - 1; i >= 0; i--)
	{
		ENetPeer* peer = &server->peers[i];
		enet_peer_disconnect_now(peer, 0);
	}
	if (server)
		enet_host_destroy(server);
}

Client::Client(std::array<int, 4> ip, int port)
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
		mThread.detach();
	}
	else {

		logcout << "[WARN/CLIENT]: Failed to connect to server!" << std::endl;
		enet_peer_reset(serverPeer);
	}
}
Client::~Client()
{
	state = false;
	if (serverPeer != nullptr)
	{
		logcout << "[INFO/CLIENT]: Disconnected from server " << serverPeer->address.host << ":" << serverPeer->address.port << "(" << ipStr << ":" << mPort << ")" << std::endl;
		enet_peer_disconnect_now(serverPeer, 0);
		serverPeer = nullptr;
	}
	if (client != nullptr)
		enet_host_destroy(client);
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
	while (state)
	{
		if (enet_host_service(client, &event, 10) > 0)
		{
			if (lastHeartBeatTime + std::chrono::seconds(5) < std::chrono::system_clock::now())
			{
				logcout << "[INFO/CLIENT]: Disconnected from server " << serverPeer->address.host << ":" << serverPeer->address.port << "(" << ipStr << ":" << mPort << ")" << std::endl;
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
					ENetPacket* packet = enet_packet_create(rspmsg.c_str(), rspmsg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(event.peer, 0, packet);
					lastHeartBeatTime = std::chrono::system_clock::now();
					logcout << "[INFO/CLIENT]: Successfully get shake hands response from server " << event.peer->address.host << ":" << event.peer->address.port << "(" << ipStr << ":" << mPort << ")" << std::endl;
				}
				if (getmsg == "Beat")
				{
					std::string rspmsg = "Beat";
					ENetPacket* packet = enet_packet_create(rspmsg.c_str(), rspmsg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(event.peer, 0, packet);
					lastHeartBeatTime = std::chrono::system_clock::now();
				}
				enet_packet_destroy(event.packet);
			}
			break;
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
		enet_host_destroy(client);
}