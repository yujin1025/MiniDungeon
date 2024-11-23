#pragma once
#include <iostream>
#include <sstream>
#include <cstdarg>  // 가변 인자 처리에 필요한 헤더
#include <cstdio>   // printf 스타일 포맷팅을 위해 사용
#include <shared_mutex> // std::shared_mutex, std::shared_lock, std::unique_lock

#define OUT

#define NAMESPACE_BEGIN(name)	namespace name {
#define NAMESPACE_END			}

/*---------------
	  Lock
---------------*/

// 매크로 정의
#define USE_MANY_LOCKS(count)	std::shared_mutex _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)

// 쓰기 락 매크로
#define WRITE_LOCK_IDX(idx)		std::unique_lock<std::shared_mutex> writeLockGuard_##idx(_locks[idx]);
#define WRITE_LOCK				WRITE_LOCK_IDX(0)

// 읽기 락 매크로
#define READ_LOCK_IDX(idx)		std::shared_lock<std::shared_mutex> readLockGuard_##idx(_locks[idx]);
#define READ_LOCK				READ_LOCK_IDX(0)

/*---------------
	  Crash
---------------*/

#define LOG_CRASH(cause) std::cerr << "Crash Cause: " << cause << std::endl;

#define CRASH(cause)                                     \
{                                                        \
    LOG_CRASH(cause);                                    \
    std::cerr << "Press Enter to exit..." << std::endl;  \
    std::cin.get();                                      \
    uint32* crash = nullptr;                             \
    __analysis_assume(crash != nullptr);                 \
    *crash = 0xDEADBEEF;                                 \
}

#define ASSERT_CRASH(expr)                  \
{                                           \
    if (!(expr))                            \
    {                                       \
        CRASH("ASSERT_CRASH: " #expr);      \
        __analysis_assume(expr);            \
    }                                       \
}

/*---------------
	  Log
----------------*/

#define LOG(msg) {                          \
    std::cout << msg << std::endl;          \
}

#define LOGF(fmt, ...) {                    \
	char buffer[256];                       \
	sprintf_s(buffer, fmt, __VA_ARGS__);      \
	std::cout << buffer << std::endl;       \
}

#define LOG_INFO()                                      \
		cout << "File: " << __FILE__                    \
              << ", Line: " << __LINE__                 \
              << ", Function: " << __func__ << endl;
