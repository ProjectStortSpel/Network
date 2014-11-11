#pragma once

#include <string>
#include <RakPeerInterface.h>
#include <PacketLogger.h>
#include <MessageIdentifiers.h>

#include <Kbhit.h>
#include <Gets.h>

using namespace RakNet;

class Client
{
public:
	Client();
	~Client();

	bool Start(void);
	int Run(void);

private:

	unsigned char GetPacketIdentifier(Packet *p);

	std::string m_password;
	std::string m_address;
	int m_clientPort;
	int m_serverPort;

	RakPeerInterface* m_client;
	RakNetStatistics* m_statistics;
	Packet* m_packet;
	PacketLogger m_logger;
};