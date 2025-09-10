#pragma once
#include "ClientPacketHandler.h"
#include "NetworkManager.h"

class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		//cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		//cout << "Connected To Server" << endl;
	}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override
	{
		ClientPacketHandler::HandlePacket(static_pointer_cast<ServerSession>(shared_from_this()), buffer, len);
	}

	virtual void OnSend(int32 len) override
	{
		//cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		auto* network = GET_SINGLE(NetworkManager);
		network->SetConnected(false);
		network->RequestReconnect();
		//cout << "Disconnected" << endl;
	}
};
