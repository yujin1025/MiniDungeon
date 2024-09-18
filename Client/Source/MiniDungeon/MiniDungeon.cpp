// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiniDungeon.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogMDNetwork);
IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, MiniDungeon, "MiniDungeon" );

/*----------------
	SendBuffer
-----------------*/

SendBuffer::SendBuffer(int32 bufferSize)
{
	_buffer.SetNum(bufferSize);
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::CopyData(void* data, int32 len)
{
	::memcpy(_buffer.GetData(), data, len);
	_writeSize = len;
}

void SendBuffer::Close(uint32 writeSize)
{
	_writeSize = writeSize;
}
