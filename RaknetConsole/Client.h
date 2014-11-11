#pragma once

#include <string>
#include <RakPeerInterface.h>
#include <PacketLogger.h>
#include <MessageIdentifiers.h>
#include <Kbhit.h>
#include <Gets.h>

#include "Global.h"

using namespace RakNet;

class Client
{
public:
	Client();
	~Client();

	bool Start(bool _isServer);
	int Run(void);

private:

	void HandleMessage(unsigned char);

	std::string m_password;
	std::string m_address;
	std::string m_userName;
	int m_clientPort;
	int m_serverPort;

	RakPeerInterface* m_client;
	RakNetStatistics* m_statistics;
	Packet* m_packet;
	PacketLogger m_logger;
	BitStream m_stream;
};