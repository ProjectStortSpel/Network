#include "stdafx.h"
#include "Server.h"

#include <mutex>


Server::Server()
{
}


Server::~Server()
{
}


ProjectStortSpel::StartResult Server::Connect()
{
	return ProjectStortSpel::UNKNOWN;
}
void Server::Disconect()
{
}

void Server::RecivePackets()
{
	RakNet::Packet* packet;
	//m_server->DeallocatePacket(m_packet),
	for (packet = m_server->Receive(); packet; packet = m_server->Receive())
	{
		//packetIdentifier = GetPacketIdentifier(m_packet);
		//if (packetIdentifer == Server_Specific_Message
		//		HandleMessage
		//else
		m_packetLock.lock();
			m_packets.push(packet);
		m_packetLock.unlock();

		
	}
}

RakNet::Packet Server::GetPacket()
{
	RakNet::Packet* p = m_packets.front();
	m_server->DeallocatePacket(m_packets.front());
	m_packets.pop();

	return *p;
}

void Server::SetOnUserConnect(std::function<void()> _function)
{
	m_onUserConnect = _function;
}

void Server::SetOnUserDisconnect(std::function<void()> _function)
{
	m_onUserDisconnect = _function;
}

void Server::SetOnUserTimeOut(std::function<void()> _function)
{
	m_onUserTimeOut = _function;
}