// RaknetConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Server.h"
#include "Client.h"
#include <Gets.h>
#include <RakSleep.h>

using namespace RakNet;

int _tmain(int argc, _TCHAR* argv[])
{
	Server* m_server = 0;
	Client* m_client = 0;

	bool m_isServer = false;

	printf("RakNet server/client test.\n\n");
	
	printf("Press 1 to start the server & client, or 2 for only client.\n");

	char input[5];

	Gets(input, sizeof(input));
	system("cls");
	if (atoi(input) == 1)
	{
		m_server = new Server();
		if (!m_server->Start())
		{
			return 0;
		}
		m_isServer = true;
	}
	
	m_client = new Client();
	if (!m_client->Start())
	{
		return 0;
	}


	while (true)
	{
		RakSleep(30);

		m_client->Run();
		if(m_isServer)
			m_server->Run();
	}










	if (m_client)
	{
		delete m_client;
		m_client = 0;
	}
	if (m_server)
	{
		delete m_server;
		m_server = 0;
	}
	return 0;
}

