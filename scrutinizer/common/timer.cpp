// timer.cpp - Johnny Bigert 2001-04-20

#if defined(TIMER) && defined(WIN32)

#include "windows.h"
#include "timer.h"

Timer::type Timer::counter() const
{
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return t.QuadPart; 
}

// jb: my 866 MHz proc returns 864e6.
Timer::type Timer::clocks_per_sec()
{
    LARGE_INTEGER t;
    QueryPerformanceFrequency(&t);
    return t.QuadPart; 
}

#endif // defined(PC_TIMER)
