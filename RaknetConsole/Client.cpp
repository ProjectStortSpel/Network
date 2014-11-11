#include "stdafx.h"
#include "Client.h"

Client::Client()
{
	m_password = "Rumpelstiltskin";
	m_address = "127.0.0.1";
	m_clientPort = 125;
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

		switch (packetIdentifier)
		{
		case ID_DISCONNECTION_NOTIFICATION:
			// Connection lost normally
			printf("ID_DISCONNECTION_NOTIFICATION\n");
			break;
		case ID_ALREADY_CONNECTED:
			// Connection lost normally
			printf("ID_ALREADY_CONNECTED with guid %" PRINTF_64_BIT_MODIFIER "u\n", m_packet->guid);
			break;
		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
			printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
			break;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
			printf("ID_REMOTE_DISCONNECTION_NOTIFICATION\n");
			break;
		case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
			printf("ID_REMOTE_CONNECTION_LOST\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
			printf("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
			break;
		case ID_CONNECTION_BANNED: // Banned from this server
			printf("We are banned from this server.\n");
			break;
		case ID_CONNECTION_ATTEMPT_FAILED:
			printf("Connection attempt failed\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			// Sorry, the server is full.  I don't do anything here but
			// A real app should tell the user
			printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
			break;

		case ID_INVALID_PASSWORD:
			printf("ID_INVALID_PASSWORD\n");
			break;

		case ID_CONNECTION_LOST:
			// Couldn't deliver a reliable packet - i.e. the other system was abnormally
			// terminated
			printf("ID_CONNECTION_LOST\n");
			break;

		case ID_CONNECTION_REQUEST_ACCEPTED:
			// This tells the client they have connected
			printf("ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s\n", m_packet->systemAddress.ToString(true), m_packet->guid.ToString());
			printf("My external address is %s\n", m_client->GetExternalID(m_packet->systemAddress).ToString(true));
			break;
		case ID_CONNECTED_PING:
		case ID_UNCONNECTED_PING:
			printf("Ping from %s\n", m_packet->systemAddress.ToString(true));
			break;
		default:
			// It's a client, so just show the message
			printf("I Am Client: %s\n", m_packet->data);
			break;
		}
	}


	return 1;
}

unsigned char Client::GetPacketIdentifier(RakNet::Packet *p)
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