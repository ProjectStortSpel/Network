#include "stdafx.h"
#include "Server.h"


Server::Server()
{
	m_password = "Rumpelstiltskin";
	m_port = 123;
	m_noOfConnections = 4;

	m_server = RakNet::RakPeerInterface::GetInstance();

}


Server::~Server()
{
	m_server->Shutdown(300);
	RakPeerInterface::DestroyInstance(m_server);
}

bool Server::Start(void)
{
	printf("Starting server!\n\n");
	printf("Port: %i\n", m_port);
	printf("Maximum clients: %i\n", m_noOfConnections);

	m_server->SetIncomingPassword(m_password.c_str(), (int)strlen(m_password.c_str()));
	m_server->SetTimeoutTime(30000, UNASSIGNED_SYSTEM_ADDRESS);
	//m_server->AttachPlugin(&m_logger);


	SocketDescriptor socketDesc[1];
	socketDesc[0].port = m_port;
	socketDesc[0].socketFamily = AF_INET;

	bool started = m_server->Startup(m_noOfConnections, socketDesc, 1) == RAKNET_STARTED;
	if (!started)
	{
		printf("Failed to start server!\n");
		return false;
	}
	
	m_server->SetMaximumIncomingConnections(m_noOfConnections);
	m_server->SetOccasionalPing(true);
	m_server->SetUnreliableTimeout(true);

	DataStructures::List<RakNetSocket2*> sockets;
	m_server->GetSockets(sockets);

	printf("\nSocket addresses used by RakNet:\n");
	for (unsigned int i = 0; i < sockets.Size(); ++i)
	{
		printf("%i. %s\n", i + 1, sockets[i]->GetBoundAddress().ToString(true));
	}

	printf("\nIP Addresses:\n");
	for (unsigned int i = 0; i < m_server->GetNumberOfAddresses(); ++i)
	{
		SystemAddress sa = m_server->GetInternalID(UNASSIGNED_SYSTEM_ADDRESS, i);
		printf("%i. %s (LAN=%i)\n", i + 1, sa.ToString(true), sa.IsLANAddress());
	}

	printf("\nGUID: %s\n", m_server->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS).ToString());

	printf("Server started!!\n");
	return true;
}


int Server::Run(void)
{
	char message[2048];

	if (_kbhit())
	{
		Gets(message, sizeof(message));
	}

	unsigned char packetIdentifier;
	for (m_packet = m_server->Receive(); m_packet; m_server->DeallocatePacket(m_packet), m_packet = m_server->Receive())
	{
		packetIdentifier = GetPacketIdentifier(m_packet);
		HandleMessage(packetIdentifier);
	}



	return 1;
}

void Server::HandleMessage(unsigned char p_packetIdentifier)
{
	switch (p_packetIdentifier)
	{
	case ID_NEW_INCOMING_CONNECTION:
	{
		break;
	}
	case ID_DISCONNECTION_NOTIFICATION:
		break;
	case ID_INCOMPATIBLE_PROTOCOL_VERSION:
		break;
	case ID_CONNECTED_PING:
	case ID_UNCONNECTED_PING:
		break;
	case ID_CONNECTION_LOST:
		break;
	case ID_USER_USERNAME:
	{
		unsigned char type;
		unsigned short length;
		char* userName;

		BitStream stream(m_packet->data, m_packet->length, false);
		stream.Read(type);
		stream.Read(length);

		userName = new char[length];
		stream.Read(userName,length);

		User u;
		u.Id				= m_users.size();
		u.Port				= m_packet->systemAddress.GetPort();
		u.RemoteAddress		= m_packet->systemAddress.ToString(false);
		u.UserName			= userName;

		m_users.emplace_back(u);

		printf("Server: New user '%s' connected from %s\n", userName, m_packet->systemAddress.ToString(true));
		break;
	}
	default:
		printf("Server got message: %s\n", m_packet->data);
		break;
	}
}