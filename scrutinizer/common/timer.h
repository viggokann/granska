/* timer.hh
 * author: Johan Carlberger
 * last change: 2000-03-29
 * comments: A simple Timer class
 */

#ifndef _timer_hh
#define _timer_hh



#ifdef TIMER

#ifdef WIN32

// jbfix: added WIN32 section for measurements on the PC
extern bool xTakeTime;


struct Timer {
    typedef __int64 type;

    void Start()	{ starttime = counter(); }
    type Get()		{ return counter() - starttime; }
    type Restart()	// returns time since last (re)start
    {
	type s = starttime;
	starttime = counter();
	return starttime - s;
    }

    static type clocks_per_sec();

private:
    type counter() const;
    type starttime;
};

#else  // WIN32

// Unix time measurements
#include <sys/time.h>
extern bool xTakeTime;
#include <iostream>

#ifdef linux

struct Timer {
  typedef unsigned long long int type; // u_int64_t
  void Start()	{ starttime = getmicros(); }
  type Get()		{ return getmicros() - starttime; }
  type Restart()	// returns time in ns since last (Re)start
  {		
    type s = starttime;
    starttime = getmicros();
    return starttime -s;
  }
  
  static type clocks_per_sec() { return 1000000; }
  
  private:
  type getmicros(){
    timeval timeVal;
    gettimeofday(&timeVal, NULL);
    return (timeVal.tv_usec + timeVal.tv_sec*(type)1000000);
  }
  type starttime;
};

#else  //linux

struct Timer {
    typedef hrtime_t type;

    void Start()	{ starttime = gethrtime(); }
    type Get()		{ return gethrtime() - starttime; }
    type Restart()	// returns time in ns since last (Re)start
    {		
        type s = starttime;
        starttime = gethrtime();
        return starttime -s;
    }

    static type clocks_per_sec() { return 1000000000; }

private:
    type starttime;
};

#endif  // linux

#endif  // WIN32

#else	// TIMER

const bool xTakeTime = 0;



struct Timer {
    typedef int type;

    void Start()		    { }
    type Get()			    { return 0; }
    type Restart()		    { return 0; }
    static type clocks_per_sec()    { return 1000; }
};

#endif  // TIMER

#endif
