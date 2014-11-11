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
		DefaultMessageIDTypes type = (DefaultMessageIDTypes)packetIdentifier;
		int hest = 0;

		switch (packetIdentifier)
		{
		case ID_DISCONNECTION_NOTIFICATION:
			// Connection lost normally
			printf("ID_DISCONNECTION_NOTIFICATION from %s\n", m_packet->systemAddress.ToString(true));;
			break;


		case ID_NEW_INCOMING_CONNECTION:
			// Somebody connected.  We have their IP now
			printf("ID_NEW_INCOMING_CONNECTION from %s with GUID %s\n", m_packet->systemAddress.ToString(true), m_packet->guid.ToString());

			printf("Remote internal IDs:\n");
			for (int index = 0; index < MAXIMUM_NUMBER_OF_INTERNAL_IDS; index++)
			{
				RakNet::SystemAddress internalId = m_server->GetInternalID(m_packet->systemAddress, index);
				if (internalId != RakNet::UNASSIGNED_SYSTEM_ADDRESS)
				{
					printf("%i. %s\n", index + 1, internalId.ToString(true));
				}
			}

			break;

		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
			printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
			break;

		case ID_CONNECTED_PING:
		case ID_UNCONNECTED_PING:
			printf("Ping from %s\n", m_packet->systemAddress.ToString(true));
			break;

		case ID_CONNECTION_LOST:
			// Couldn't deliver a reliable packet - i.e. the other system was abnormally
			// terminated
			printf("ID_CONNECTION_LOST from %s\n", m_packet->systemAddress.ToString(true));;
			break;

		default:
			// The server knows the static data of all clients, so we can prefix the message
			// With the name data
			printf("I Am Server: %s\n", m_packet->data);

			// Relay the message.  We prefix the name for other clients.  This demonstrates
			// That messages can be changed on the server before being broadcast
			// Sending is the same as before
			sprintf_s(message, "%s", m_packet->data);
			m_server->Send(message, (const int)strlen(message) + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_packet->systemAddress, true);

			break;
		}
	}


	return 1;
}

unsigned char Server::GetPacketIdentifier(RakNet::Packet *p)
{
	if (p == 0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char)p->data[0];
}