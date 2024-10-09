﻿#include "pch.h"
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
#include "SMTPManager.h"

enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
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

int main()
{
	try
	{
		SMTPManager smtp;
		smtp.SetDNSAddress("google.co.kr");
		smtp.SetEmailFrom("hans4809@gmail.com");
		smtp.SetEmailTo("hondaestudy@gmail.com");
		smtp.SetMessage("TEST");
		smtp.SetServerPort(25);
		smtp.SetSMTPServer("smtp.gmail.com");
		smtp.SetEmailSubject("MAIL_TEST");

		smtp.Transport();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}




	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={MySQL ODBC 8.4 ANSI Driver};Server=database-1.c5y046mwe85d.ap-northeast-2.rds.amazonaws.com;Database=MDDB;UID=hans4809;PWD=*gyqls124;"));

	ServerPacketHandler::Init();

	ServerServiceRef service = make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[=]() { return make_shared<GameSession>(); }, // TODO : SessionManager 등
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}

	// Main Thread
	//DoWorkerJob(service);

	//GRoom->DoAsync(&Room::UpdateTick);

	while (true)
	{
		//Protocol::S_CHAT pkt;
		//pkt.set_msg("HelloWorld");
		//auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);

		//GSessionManager.Broadcast(sendBuffer);
		this_thread::sleep_for(0.1s);
	}

	GThreadManager->Join();
}