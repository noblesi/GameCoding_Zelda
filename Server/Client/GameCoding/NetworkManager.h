#pragma once

using ServerSessionRef = shared_ptr<class ServerSession>;

class NetworkManager
{
	DECLARE_SINGLE(NetworkManager)

public:
	void Init();
	void Update();

	void RequestReconnect();
	void SetConnected(bool value) { _connected = value; }

	ServerSessionRef CreateSession();
	void SendPacket(SendBufferRef sendBuffer);

private:
	ClientServiceRef _service;
	ServerSessionRef _session;
	atomic<bool> _connected = false;
	atomic<bool> _reconnectRequested = false;
};

