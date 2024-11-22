#pragma once

#include <mutex>
#include <atomic>

using BYTE = unsigned char;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

enum class ENodeState
{
	Success,
	Failure,
	Abort,
	Running,
};

enum class EBlackboardKey
{
	Target,
	Position,
};

struct Vector3
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	// == 연산자 오버로딩
	bool operator==(const Vector3& other) const 
	{
		return x == other.x && y == other.y && z == other.z;
	}

	// != 연산자 오버로딩 (선택 사항)
	bool operator!=(const Vector3& other) const 
	{
		return !(*this == other);
	}
};

struct Vector2
{
	float x = 0.0f;
	float y = 0.0f;

	bool operator==(const Vector2& other) const 
	{
		return x == other.x && y == other.y;
	}

	// != 연산자 오버로딩 (선택 사항)
	bool operator!=(const Vector2& other) const 
	{
		return !(*this == other);
	}
};

//template<typename T>
//using Atomic = std::atomic<T>;
//using Mutex = std::mutex;
//using CondVar = std::condition_variable;
//using UniqueLock = std::unique_lock<std::mutex>;
//using LockGuard = std::lock_guard<std::mutex>;

#define USING_SHARED_PTR(name)	using name##Ref = std::shared_ptr<class name>;

USING_SHARED_PTR(IocpCore);
USING_SHARED_PTR(IocpObject);
USING_SHARED_PTR(Session);
USING_SHARED_PTR(PacketSession);
USING_SHARED_PTR(Listener);
USING_SHARED_PTR(ServerService);
USING_SHARED_PTR(ClientService);
USING_SHARED_PTR(SendBuffer);
USING_SHARED_PTR(Job);
USING_SHARED_PTR(JobQueue);

#define size16(val)		static_cast<int16>(sizeof(val))
#define size32(val)		static_cast<int32>(sizeof(val))
#define len16(arr)		static_cast<int16>(sizeof(arr)/sizeof(arr[0]))
#define len32(arr)		static_cast<int32>(sizeof(arr)/sizeof(arr[0]))

//#define _STOMP