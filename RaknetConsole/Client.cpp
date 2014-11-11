#include "stdafx.h"
#include "Client.h"

Client::Client()
{
	m_password = "Rumpelstiltskin";
	m_address = "127.0.0.1";
	m_clientPort = 127;
	m_serverPort = 123;

	m_client = RakNet::RakPeerInterface::GetInstance();
}


Client::~Client()
{
	m_client->Shutdown(300);
	RakPeerInterface::DestroyInstance(m_client);
}

bool Client::Start(void)
{
	printf("Starting client!\n");

	printf("\nMy IP addresses:\n");
	for (unsigned int i = 0; i < m_client->GetNumberOfAddresses(); ++i)
	{
		printf("%i, %s\n", i + 1, m_client->GetLocalIP(i));
	}

	printf("My GUID is %s\n", m_client->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS).ToString());

	m_client->AllowConnectionResponseIPMigration(false);

	SocketDescriptor socketDesc(m_clientPort, 0);
	socketDesc.socketFamily = AF_INET;

	m_client->Startup(8, &socketDesc, 1);
	m_client->SetOccasionalPing(true);

	ConnectionAttemptResult car = m_client->Connect(m_address.c_str(), m_serverPort, m_password.c_str(), (int)strlen(m_password.c_str()));
	//RakAssert(car == RakNet::CONNECTION_ATTEMPT_STARTED);

	if (car == RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		printf("Connecting to: %s:%i\n", m_address.c_str(), m_serverPort);
	}
	else
	{
		printf("Bad connection, exiting!\n");
		return false;
	}

	printf("Client started!!\n");

	return true;

}


int Client::Run(void)
{
	char message[2048];

	if (_kbhit())
	{
		Gets(message, sizeof(message));
		m_client->Send(message, (const int)strlen(message) + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}

	unsigned char packetIdentifier;
	for (m_packet = m_client->Receive(); m_packet; m_client->DeallocatePacket(m_packet), m_packet = m_client->Receive())
	{
		packetIdentifier = GetPacketIdentifier(m_packet);
		HandleMessage(packetIdentifier);
	}


	return 1;
}


void Client::HandleMessage(unsigned char p_packetIdentifier)
{
	switch (p_packetIdentifier)
	{
	case ID_DISCONNECTION_NOTIFICATION:
		break;
	case ID_ALREADY_CONNECTED:
		break;
	case ID_INCOMPATIBLE_PROTOCOL_VERSION:
		break;
	case ID_REMOTE_DISCONNECTION_NOTIFICATION:
		break;
	case ID_REMOTE_CONNECTION_LOST:
		break;
	case ID_REMOTE_NEW_INCOMING_CONNECTION:
		break;
	case ID_CONNECTION_BANNED: // Banned from this server
		break;
	case ID_CONNECTION_ATTEMPT_FAILED:
		break;
	case ID_NO_FREE_INCOMING_CONNECTIONS:
		break;
	case ID_INVALID_PASSWORD:
		break;
	case ID_CONNECTION_LOST:
		break;
	case ID_CONNECTION_REQUEST_ACCEPTED:
	{
		printf("Client: Connected to %s:%i.\n", m_address.c_str(), m_serverPort);
		BitStream stream;
		unsigned char typeID = ID_USER_USERNAME;
		stream.Write(typeID);
		stream.Write((unsigned short)(strlen("ThePettsoN2") + 1));
		stream.Write("ThePettsoN2", strlen("ThePettsoN2") + 1);
		m_client->Send(&stream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

		break;
	}
	case ID_CONNECTED_PING:
	case ID_UNCONNECTED_PING:
		break;
	default:
		// It's a client, so just show the message
		printf("I Am Client: %s\n", m_packet->data);
		break;
	}
}