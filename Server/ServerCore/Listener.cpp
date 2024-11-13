#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"

/*--------------
	Listener
---------------*/

Listener::~Listener()
{
	SocketUtils::Close(_socket);

	for (AcceptEvent* acceptEvent : _acceptEvents)
	{
		// TODO

		delete(acceptEvent);
	}
}

bool Listener::StartAccept(ServerServiceRef service)
{
	_service = service;
	if (_service == nullptr)
		return false;

	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET)
		return false;

	if (_service->GetIocpCore()->Register(shared_from_this()) == false)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::SetLinger(_socket, 0, 0) == false)
		return false;

#ifdef _DEBUG
	// 로컬인 경우
	if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false)
		return false;
#else
	// 로컬 아닌 경우
	if (SocketUtils::BindAnyAddress(_socket, _service->GetNetAddress().GetPort()) == false)
		return false;
#endif

	if (SocketUtils::Listen(_socket) == false)
		return false;

	const int32 acceptCount = _service->GetMaxSessionCount();
	for (int32 i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = new AcceptEvent();
		acceptEvent->owner = shared_from_this();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

void Listener::CloseSocket()
{
	SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	LOG("Dispatching accept event");
	ASSERT_CRASH(iocpEvent->eventType == EventType::Accept);
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	LOG("Processing accept event");
	ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	LOG("Registering accept event...");
	SessionRef session = _service->CreateSession(); // Register IOCP

	LOG("Initializing accept event");
	acceptEvent->Init();
	acceptEvent->session = session;

	DWORD bytesReceived = 0;
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			LOG("AcceptEx failed with error: " << errorCode);
			// 일단 다시 Accept 걸어준다
			RegisterAccept(acceptEvent);
		}
	}
	else
	{
		LOG("AcceptEx initiated successfully");
	}
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	LOG("Processing accept event");
	SessionRef session = acceptEvent->session;

	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
	{
		LOG("Failed to update accept socket");
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(sockAddress);
	if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		LOG("getpeername failed with error: " << WSAGetLastError());
		RegisterAccept(acceptEvent);
		return;
	}

	LOG("Setting network address for session");
	session->SetNetAddress(NetAddress(sockAddress));
	LOG("Processing connect for session");
	session->ProcessConnect();
	LOG("Registering accept for next connection");
	RegisterAccept(acceptEvent);
}