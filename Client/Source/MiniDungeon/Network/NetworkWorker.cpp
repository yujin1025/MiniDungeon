// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/NetworkWorker.h"
#include "Sockets.h"
#include "Serialization/ArrayWriter.h"
#include "PacketSession.h"

RecvWorker::RecvWorker(FSocket* socket, TSharedPtr<class PacketSession> session) : Socket(socket), SessionRef(session)
{
	Thread = FRunnableThread::Create(this, TEXT("RecvWorkerThread"));
}

RecvWorker::~RecvWorker()
{
}

bool RecvWorker::Init()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Thread Init")));
	return true;
}

uint32 RecvWorker::Run()
{
	while (Running)
	{
		TArray<uint8> packet;

		if (ReceivePacket(packet))
		{
			if (TSharedPtr<PacketSession> session = SessionRef.Pin())
			{
				session->RecvPacketQueue.Enqueue(packet);
			}
		}
	}

	return 0;
}

void RecvWorker::Exit()
{
}

void RecvWorker::Destroy()
{
	Running = false;
}

bool RecvWorker::ReceivePacket(TArray<uint8>& outPacket)
{
	// 패킷 헤더 파싱
	const int32 headerSize = sizeof(FPacketHeader);
	TArray<uint8> headerBuffer;
	headerBuffer.AddZeroed(headerSize);

	if (ReceiveDesiredBytes(headerBuffer.GetData(), headerSize) == false)
		return false;

	// ID, Size 추출
	FPacketHeader header;
	{
		FMemoryReader reader(headerBuffer);
		reader << header;
		UE_LOG(LogTemp, Log, TEXT("Recv PacketID : %d, PacketSize : %d"), header.PacketID, header.PacketSize);
	}

	// 패킷 헤더 복사
	outPacket = headerBuffer;

	// 패킷 내용 파싱
	TArray<uint8> payloadBuffer;
	const int32 payloadSize = header.PacketSize - headerSize;
	if (payloadSize == 0)
		return true;

	outPacket.AddZeroed(payloadSize);

	if (ReceiveDesiredBytes(&outPacket[headerSize], payloadSize))
		return true;

	return false;
}

bool RecvWorker::ReceiveDesiredBytes(uint8* results, int32 size)
{
	uint32 pendingDataSize;
	if(Socket->HasPendingData(pendingDataSize) == false || pendingDataSize <= 0)
		return false;

	int32 offset = 0;

	while (size > 0)
	{
		int32 numRead = 0;
		Socket->Recv(results + offset, size, OUT numRead);
		check(numRead <= size);

		if(numRead <= 0)
			return false;

		offset += numRead;
		size -= numRead;
	}

	return true;
}

SendWorker::SendWorker(FSocket* socket, TSharedPtr<class PacketSession> session) : Socket(socket), SessionRef(session)
{
	Thread = FRunnableThread::Create(this, TEXT("SendWorkerThread"));
}

SendWorker::~SendWorker()
{
}

bool SendWorker::Init()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Send Thread Init")));
	return true;
}

uint32 SendWorker::Run()
{
	while (Running)
	{
		SendBufferRef sendBuffer;

		if (TSharedPtr<PacketSession> session = SessionRef.Pin())
		{
			if(session->SendPacketQueue.Dequeue(OUT sendBuffer))
			{
				SendPacket(sendBuffer);
			}
		}

		//Sleep?
	}

	return 0;
}

void SendWorker::Exit()
{
}

bool SendWorker::SendPacket(SendBufferRef sendBuffer)
{
	if (SendDesiredBytes(sendBuffer->Buffer(), sendBuffer->WriteSize()) == false)
	{
		UE_LOG(LogTemp, Log, TEXT("SendPacket Failed"));
		return false;
	}

	return true;
}

void SendWorker::Destroy()
{
	Running = false;
}

bool SendWorker::SendDesiredBytes(const uint8* buffer, int32 size)
{
	while (size > 0)
	{
		int32 bytesSent = 0;
		if(Socket->Send(buffer, size, OUT bytesSent) == false)
			return false;

		size -= bytesSent;
		buffer += bytesSent;
	}

	return true;
}