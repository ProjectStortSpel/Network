#ifndef GLOBAL_H
#define GLOBAL_H

#include <MessageIdentifiers.h>
#include <BitStream.h>

enum MessageIDType
{
	ID_USER_USERNAME = ID_USER_PACKET_ENUM,
	ID_USER_CHAT_MESSAGE
};

static unsigned char GetPacketIdentifier(RakNet::Packet *p)
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

static void PackMessageStream(RakNet::BitStream& _stream, unsigned char _type, const char* _message)
{
	_stream.Reset();
	unsigned short length = strlen(_message) + 1;

	_stream.Write(_type);
	_stream.Write(length);
	_stream.Write(_message, length);
}


static char* UnpackMessageStream(MessageIDType _type, RakNet::Packet* _packet)
{
	RakNet::BitStream stream(_packet->data, _packet->length, false);

	unsigned char type;
	unsigned short length;
	char* message;

	stream.Read(type);
	stream.Read(length);

	message = new char[length];
	stream.Read(message, length);

	return message;

}

#endif