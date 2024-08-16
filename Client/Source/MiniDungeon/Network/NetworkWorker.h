// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "MiniDungeon.h"

class FSocket;

struct MINIDUNGEON_API FPacketHeader
{
	FPacketHeader() : PacketSize(0), PacketID(0)
	{
	}

	FPacketHeader(uint16 packetSize, uint16 packetID) : PacketSize(packetSize), PacketID(packetID)
	{
	}

	friend FArchive& operator<<(FArchive& ar, FPacketHeader& header)
	{
		ar << header.PacketSize;
		ar << header.PacketID;
		return ar;
	}

	uint16 PacketSize;
	uint16 PacketID;
};

class MINIDUNGEON_API RecvWorker : public FRunnable
{
public:
	RecvWorker(FSocket* socket, TSharedPtr<class PacketSession> session);
	~RecvWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;

	void Destroy();

private:
	bool ReceivePacket(TArray<uint8>& outPacket);
	bool ReceiveDesiredBytes(uint8* results, int32 size);

protected:
	FRunnableThread* Thread = nullptr;
	bool Running = true;
	FSocket* Socket;
	TWeakPtr<class PacketSession> SessionRef;
};

class MINIDUNGEON_API SendWorker : public FRunnable
{
public:
	SendWorker(FSocket* socket, TSharedPtr<class PacketSession> session);
	~SendWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;

	bool SendPacket(SendBufferRef sendBuffer);

	void Destroy();

private:
	bool SendDesiredBytes(const uint8* buffer, int32 size);

protected:
	FRunnableThread* Thread = nullptr;
	bool Running = true;
	FSocket* Socket;
	TWeakPtr<class PacketSession> SessionRef;
};
