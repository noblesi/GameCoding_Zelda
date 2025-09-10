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
	bool IsConnected() const { return _connected; }
	void SetServerInfo(const wstring& ip, uint16 port);

	ServerSessionRef CreateSession();
	void SendPacket(SendBufferRef sendBuffer);

private:
	ClientServiceRef _service;
	ServerSessionRef _session;
	wstring _ip;
	uint16 _port = 0;
	atomic<bool> _connected = false;
	atomic<bool> _reconnectRequested = false;
};

