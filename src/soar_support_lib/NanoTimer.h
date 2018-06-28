/**
* \file NanoTimer.h
* \brief Declaration and inline implementation of class NanoTimer
*
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*/

#ifndef NANOTIMER_H
#define NANOTIMER_H

#if defined(_MSC_VER)
#  define NOMINMAX      // workaround a bug in windows.h
#  include <windows.h>
#else
#  include <sys/time.h>
#endif

/**
* \ingroup SoarSupport
* \brief A flexible timer with nano seconds resolution
*
* <DIV class="groupHeader">Copyright</DIV>
* (C) Karsten Isakovic, Berlin  10.03.2015 ( Karsten.Isakovic@web.de )
* <HR />
*
* <H2 class="groupheader">Include</H2>
*/
class NanoTimer
{
private:
#if defined(_MSC_VER)
	float         _nano_secs;
	long          _call_count;
	LARGE_INTEGER _start;
	float         _inv_freq;
#else
	float         _nano_secs;
	long          _call_count;
	timeval       _start;
#endif

public:
	inline NanoTimer() {
#if defined(_MSC_VER)		  
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		_inv_freq = 1000000.0f / static_cast<float>(freq.QuadPart);		
#endif
		_nano_secs = 0;
		_call_count= 0;
	}


	inline void Clear()
	{
		_nano_secs = 0;
		_call_count= 0;
	}

	inline void Start()
	{
#if defined(_MSC_VER)
		QueryPerformanceCounter(&_start);
#else
		gettimeofday(&_start, 0);
#endif
	};

	inline void Stop()
	{
#if defined(_MSC_VER)
		LARGE_INTEGER stop;

		QueryPerformanceCounter(&stop);
		float nSec = (stop.QuadPart - _start.QuadPart) * _inv_freq;
#else
		timeval       stop;
		gettimeofday(&stop, 0);

		float nSec = stop.tv_sec - _start.tv_sec + (stop.tv_usec - _start.tv_usec);		
#endif
		_nano_secs  += nSec;
		_call_count++;
	};

	inline float GetNanoSeconds() const
	{
		return _nano_secs;
	};

	inline long GetCallCount() const
	{
		return _call_count;
	};

	inline float GetAverageNanoSeconds() const
	{
		if (_call_count>0)
			return _nano_secs/_call_count;
		else
			return _nano_secs;
	};
};

#endif // NANOTIMER_H