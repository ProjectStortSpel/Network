#pragma once

#include <string>
#include <vector>
#include <RakPeerInterface.h>
#include <PacketLogger.h>
#include <Kbhit.h>
#include <Gets.h>
#include <MessageIdentifiers.h>

#include "Global.h"

using namespace RakNet;

enum UserLevels
{
	Normal = 0,
	Moderator = 1,
	Admin = 2,
	Owner = 3
};

struct User
{
	unsigned int Id;
	unsigned int Port;
	std::string RemoteAddress;
	std::string UserName;
	UserLevels UserLevel;
};


class Server
{
public:
	Server();
	~Server();

	bool Start(void);
	int Run(void);

private:

	void HandleMessage(unsigned char);

	std::string m_password;
	int m_port;
	int m_noOfConnections;

	std::vector<User> m_users;

	RakPeerInterface* m_server;
	RakNetStatistics* m_statistics;
	Packet* m_packet;
	PacketLogger m_logger;
};


