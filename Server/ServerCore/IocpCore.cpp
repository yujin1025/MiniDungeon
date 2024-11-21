#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

/*--------------
	IocpCore
---------------*/

IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef iocpObject)
{
	LOG("Registering IOCP object...");
	if (::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /*key*/0, 0) == NULL)
	{
		LOG("Failed to register IOCP object with error: " << GetLastError());
		return false;
	}
	LOG("IOCP object registered successfully");
	return true;
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;	
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocpHandle, OUT &numOfBytes, OUT &key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		IocpObjectRef iocpObject = iocpEvent->owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			LOG("GetQueuedCompletionStatus failed with error: " << errCode);
			IocpObjectRef iocpObject = iocpEvent ? iocpEvent->owner : nullptr;
			if (iocpObject)
			{
				iocpObject->Dispatch(iocpEvent, numOfBytes);
			}
			else
			{
				LOG("IocpEvent owner is null, cannot dispatch");
			}
			break;
		}
	}

	return true;
}
