#pragma once

#include <string>
#include <RakPeerInterface.h>
#include <PacketLogger.h>
#include <Kbhit.h>
#include <Gets.h>
#include <MessageIdentifiers.h>

using namespace RakNet;

class Server
{
public:
	Server();
	~Server();

	bool Start(void);
	int Run(void);

private:

	unsigned char GetPacketIdentifier(Packet *p);

	std::string m_password;
	int m_port;
	int m_noOfConnections;

	RakPeerInterface* m_server;
	RakNetStatistics* m_statistics;
	Packet* m_packet;
	PacketLogger m_logger;
};


