#pragma once

using ServerSessionRef = std::shared_ptr<class ServerSession>;

class NetworkManager
{
	DECLARE_SINGLE(NetworkManager)

public:
	void Init();
	void Update();

	void RequestReconnect();
	void SetConnected(bool value) { _connected = value; }
	bool IsConnected() const { return _connected; }
	void SetServerInfo(const std::wstring& ip, uint16 port);

	ServerSessionRef CreateSession();
	void SendPacket(SendBufferRef sendBuffer);

private:
	ClientServiceRef _service;
	ServerSessionRef _session;
	std::wstring _ip;
	uint16 _port = 0;
	std::atomic<bool> _connected = false;
	std::atomic<bool> _reconnectRequested = false;
};

