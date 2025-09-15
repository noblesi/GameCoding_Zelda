#include "pch.h"
#include "NetworkManager.h"
#include "Service.h"
#include "ThreadManager.h"
#include "ServerSession.h"
#include <thread>
#include "Logger.h"

void NetworkManager::Init()
{
	SocketUtils::Init();

	SetServerInfo(L"127.0.0.1", 7777);

	const int32 maxRetry = 5;
	bool started = false;
	for (int32 i = 0; i < maxRetry; i++)
	{
		if (_service->Start())
		{
			Logger::Info("[NetworkManager] Connected to server");
			_connected = true;
			started = true;
			break;
		}

		Logger::Warn("[NetworkManager] Start failed. retry " + std::to_string(i + 1) + "/" + std::to_string(maxRetry));

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	if (started == false)
		Logger::Error("[NetworkManager] Failed to connect to server");
}

void NetworkManager::Update()
{
	_service->GetIocpCore()->Dispatch(0);

	if (_session == nullptr || _session->IsConnected() == false)
		_reconnectRequested = true;

	if (_reconnectRequested)
	{
		if (_service->Start())
		{
			Logger::Info("[NetworkManager] Reconnected to server");
			_reconnectRequested = false;
			_connected = true;
		}
		else
		{
			Logger::Warn("[NetworkManager] Reconnect attempt failed");
		}
	}
}

void NetworkManager::SetServerInfo(const std::wstring& ip, uint16 port)
{
	_ip = ip;
	_port = port;
	_service = std::make_shared<ClientService>(
		NetAddress(ip, port),
		std::make_shared<IocpCore>(),
		[=]() { return CreateSession(); },
		1);
}

ServerSessionRef NetworkManager::CreateSession()
{
	return _session = std::make_shared<ServerSession>();
}

void NetworkManager::SendPacket(SendBufferRef sendBuffer)
{
	if (_session)
		_session->Send(sendBuffer);
}

void NetworkManager::RequestReconnect()
{
	_connected = false;
	_reconnectRequested = true;
	_session.reset();
}
