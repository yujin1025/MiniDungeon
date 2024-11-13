#pragma once

#define OUT

#define NAMESPACE_BEGIN(name)	namespace name {
#define NAMESPACE_END			}

/*---------------
	  Lock
---------------*/

#define USE_MANY_LOCKS(count)	mutex _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define	WRITE_LOCK_IDX(idx)		lock_guard<mutex> lockGuard_##idx(_locks[idx]);
#define WRITE_LOCK				WRITE_LOCK_IDX(0)

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