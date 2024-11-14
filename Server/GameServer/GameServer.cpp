#include "pch.h"
#include <iostream>
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include <tchar.h>
#include "Job.h"
#include "Protocol.pb.h"
#include "Room.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "AuthManager.h"
#include "Lobby.h"

enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
	LOG("DoWorkerJob started");
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// 예약된 일감 처리
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();
	}
}

void DoAuthManagerJob()
{
	LOG("DoAuthManagerJob started");
	while (true)
	{
		AuthManager& auth = AuthManager::GetInstance();
		auth.RemoveExpiredWaiters();
		this_thread::sleep_for(1s);
	}
}

int main()
{
	LOG("Main function started");
	//AuthManager& auth = AuthManager::GetInstance();
	//auth.AddAuthWaiter("hondaestudy@gmail.com");

	LOG("Connecting to database");
	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={MySQL ODBC 8.4 ANSI Driver};Server=database-1.c5y046mwe85d.ap-northeast-2.rds.amazonaws.com;Database=MDDB;UID=hans4809;PWD=*gyqls124;"));
	LOG("Database connected successfully");

	ServerPacketHandler::Init();
	LOG("ServerPacketHandler initialized");


#ifdef _DEBUG
	// 로컬로 돌릴 경우
	LOG("Creating server service for local address");
	ServerServiceRef service = make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[=]() { return make_shared<GameSession>(); }, // TODO : SessionManager 등
		100);
#else
	// 로컬 아닌 경우
	ServerServiceRef service = make_shared<ServerService>(
		NetAddress(L"172.31.15.71", 7777),
		make_shared<IocpCore>(),
		[=]() { return make_shared<GameSession>(); }, // TODO : SessionManager 등
		10);
#endif

	LOG("Starting server service");
	ASSERT_CRASH(service->Start());
	LOG("Server service started successfully");

	for (int32 i = 0; i < 2; i++)
	{
		LOG("Launching worker thread");
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}

	LOG("Launching AuthManager job thread");
	GThreadManager->Launch([]()
		{
			DoAuthManagerJob();
		});

	// Main Thread
	//DoWorkerJob(service);

	//GRoom->DoAsync(&Room::UpdateTick);

	while (true)
	{
		//Protocol::S_CHAT pkt;
		//pkt.set_msg("HelloWorld");
		//auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);

		//GSessionManager.Broadcast(sendBuffer);
	}

	LOG("Joining all threads");
	GThreadManager->Join();
	LOG("Main function ended");
}
