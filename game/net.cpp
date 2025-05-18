#include"net.h"
#include<iostream>
#include"../wrapper/LogCoutTime.h"

Server::Server()
{
	address.host = ENET_HOST_ANY;
	for (int port = 2000; port <= 3000; port++)
	{
		address.port = port;
		server = enet_host_create(&address, 32, 2, 0, 0);
		if (server)
		{
			std::cout << "[INFO]: Successfully established a server on port " << port << std::endl;
			useful = true;
			return;
		}
	}
	LCT();
	std::cout << "[WARN]: Failed to establish a server!" << std::endl;
}
Server::Server(int port)
{
	address.host = ENET_HOST_ANY;
	address.port = port;
	server = enet_host_create(&address, 32, 2, 0, 0);
	if (server)
	{
		std::cout << "[INFO]: Successfully established a server on port " << port << std::endl;
		useful = true;
		return;
	}
	LCT();
	std::cout << "[WARN]: Failed to establish a server on port " << port << " !" << std::endl;
}
bool Server::getUseful()
{
	return useful;
}
