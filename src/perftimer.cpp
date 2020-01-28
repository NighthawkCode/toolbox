#include "perftimer.h"
#include <time.h> 
#include <sys/time.h>

// Calculate the conversion factors for seconds, milliseconds and microseconds.
// Only call this once (or when absolutely needed) since it does double precision divides.
void CalcPerfConversions( double& conversionS, double& conversionMS, double& conversionUS )
{
    // Presume the clock frequency is nanoseconds.
    conversionUS = 1.0;         // convert nanoseconds to microseconds
    conversionUS /= 1000.0;
    conversionMS = 1.0;         // convet nanoseconds to milliseconds
    conversionMS /= 1000000.0;
    conversionS = 1.0;          // convert nanoseconds to seconds
    conversionS /= 1000000000.0;
}

// Get nanoseconds.
uint64_t GetPerfTimeNanoseconds()
{
#if 0 // defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
    // CLOCK_REALTIME, CLOCK_MONOTONIC, CLOCK_PROCESS_CPUTIME_ID, CLOCK_THREAD_CPUTIME_ID
    struct timespec t;
    t.tv_sec = t.tv_nsec = 0;
    clock_gettime( CLOCK_MONOTONIC, &t );
    return (((int64_t)(t.tv_sec)) * 1000000000LL) + t.tv_nsec;    
#else
    // POSIX clocks are not supported in this flavor of linux.
    struct timeval t;
    t.tv_sec = t.tv_usec = 0;
    gettimeofday( &t, 0 );
    return (((int64_t)(t.tv_sec)) * 1000000000LL) + (((int64_t)(t.tv_usec)) * 1000LL);
#endif
    return 0;
}


int64_t PerfTimer::m_adjust = 0;      // Adjustment time it takes to Start and Stop
double PerfTimer::m_conversionS = 1;    // Conversion factor for seconds.
double PerfTimer::m_conversionMS = 1;   // Conversion factor for milliseconds.
double PerfTimer::m_conversionUS = 1;   // Conversion factor for microseconds.

PerfTimer::PerfTimer()
{
  if( m_conversionUS == m_conversionMS ) { // Initialize conversions.
    CalcPerfConversions( m_conversionS, m_conversionMS, m_conversionUS ); 
    if( m_conversionUS == m_conversionMS ) {
      m_conversionUS = 1; 
            m_conversionMS = 1; // Timer will be useless but will not cause divide by zero
    }
    Adjust();
  }
  Reset();
}

void PerfTimer::Reset()
{
  m_start = 0; 
}

void PerfTimer::Adjust()   	// Time it takes to start and stop
{
  m_start = 0; 
  m_adjust = 0;
  Start();              	// Time a Stop
  Stop();     
  m_adjust = m_start;
}

void PerfTimer::Start( bool bReset )  
{                   		// Start from current value or optionally from 0
  uint64_t perftime = GetPerfTimeNanoseconds();
  if( (!bReset) && (m_start < 0) )
    m_start += perftime;    // Starting with an accumulated time
  else 
    m_start = perftime;     // Starting from 0
}

void PerfTimer::Stop()      // Stop timing. Use Start afterwards to continue
{
  if( m_start <= 0 ) {
    return;             	// Was not running
  }
  uint64_t perftime = GetPerfTimeNanoseconds();
  m_start += -perftime;     // Stopped timer keeps elapsed timer ticks as a negative 
  if( m_start < m_adjust )  // Do not overflow
    m_start -= m_adjust;    // Adjust for amount time that timer code takes to run
  else 
    m_start = 0;          	// Stop must have been called directly after Start (and timer code is executed in zero time!)
}

// Split (get elapsed time, just like a stopwatch, timer does not stop)
double PerfTimer::Elapsed()   // Returns elapsed time in seconds
{
  int64_t time;   
  if( m_start > 0 ) {                 
    uint64_t perftime = GetPerfTimeNanoseconds(); // Timer is still running
    time = (m_start - perftime) - m_adjust;
  }
  else {  
    time = m_start;         // Timer has stopped
  }
  double t = (-time);
  return t * m_conversionS;
}

double PerfTimer::Elapsedms()   // Returns elapsed time in milliseconds
{
  int64_t time;   
  if( m_start > 0 ) {                     
    uint64_t perftime = GetPerfTimeNanoseconds();   // Timer is still running
    time = (m_start - perftime) - m_adjust;
  }
  else {                
    time = m_start;         // Timer has stopped
  }
  double t = (-time);
  return t * m_conversionMS;
}

double PerfTimer::Elapsedus() // Returns elapsed time in microseconds
{ 
  int64_t time;   
  if( m_start > 0 ) {               
    uint64_t perftime = GetPerfTimeNanoseconds(); // Timer is still running
    time = (m_start - perftime) - m_adjust;
  }
  else {                
    time = m_start;         // Timer has stopped
  }
  double t = (-time);
  return t * m_conversionUS;
}

