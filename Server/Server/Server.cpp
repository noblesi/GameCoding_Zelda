#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
using namespace std;
#include <atomic>
#include <mutex>
#include "ThreadManager.h"
#include "SocketUtils.h"
#include "Listener.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ServerPacketHandler.h"
#include "GameRoom.h"
#include <csignal>
#include <chrono>
#include "Shutdown.h"

static atomic<bool> gIsRunning(true); // 서버 루프 실행 여부 플래그

// 외부에서 서버 종요를 요청할 떄 호출되는 함수
void RequestShutdown() { gIsRunning.store(false); }

// OS 신호(SIGINT 등)를 받아 종료를 요청하는 핸들러
static void SignaleHandler(int) { RequestShutdown(); }

int main()
{
	SocketUtils::Init();
	GRoom->Init();

	// Ctrl + C 또는 시스템 종료 신호에 반응하도록 설정
	signal(SIGINT, SignaleHandler);
	signal(SIGTERM, SignaleHandler);

	ServerServiceRef service = make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[](){ return make_shared<GameSession>(); }, // TODO : SessionManager 등
		100);

	assert(service->Start());

	const chrono::milliseconds frameDuration(16); // 60FPS 기준 프레임 시간
	while (gIsRunning.load())
	{
		auto frameStart = chrono::steady_clock::now();

		service->GetIocpCore()->Dispatch(0); // 네트워크 이벤트 처리
		GRoom->Update(); // 게임로직 업데이트

		auto frameEnd = chrono::steady_clock::now();
		auto workTime = frameEnd - frameStart;
		if (workTime < frameDuration)
			this_thread::sleep_for(frameDuration - workTime); // 남은 시간 대기
	}

	service->CloseService();
	GThreadManager->Join();
	
	// 윈속 종료
	SocketUtils::Clear();
	return 0;
}