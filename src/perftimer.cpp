#include "perftimer.h"

#include <sys/resource.h>
#include <sys/time.h>
#include <time.h>

// Get nanoseconds.
uint64_t GetPerfTimeNanoseconds() {
  // https://stackoverflow.com/questions/12392278/measure-time-in-linux-time-vs-clock-vs-getrusage-vs-clock-gettime-vs-gettimeof
#if 1  // defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
  // CLOCK_REALTIME, CLOCK_MONOTONIC, CLOCK_PROCESS_CPUTIME_ID, CLOCK_THREAD_CPUTIME_ID
  struct timespec t;
  t.tv_sec = t.tv_nsec = 0;
  clock_gettime(CLOCK_MONOTONIC, &t);
  return (((int64_t)(t.tv_sec)) * 1000000000LL) + t.tv_nsec;
#else
  // POSIX clocks are not supported in this flavor of linux.
  struct timeval t;
  t.tv_sec = t.tv_usec = 0;
  gettimeofday(&t, 0);
  return (((int64_t)(t.tv_sec)) * 1000000000LL) + (((int64_t)(t.tv_usec)) * 1000LL);
#endif
}

int64_t PerfTimer::m_adjust = 0;  // Adjustment time it takes to Start and Stop

PerfTimer::PerfTimer() {
  if (m_adjust == 0) {  // Initialize
    Adjust();
  }
  Reset();
}

void PerfTimer::Reset() { m_start = 0; }

void PerfTimer::Adjust()  // Time it takes to start and stop
{
  m_start = 0;
  m_adjust = 0;
  Start();  // Time a Stop
  Stop();
  m_adjust = m_start;
}

void PerfTimer::Start(bool bReset) {  // Start from current value or optionally from 0
  uint64_t perftime = GetPerfTimeNanoseconds();
  if ((!bReset) && (m_start < 0))
    m_start += perftime;  // Starting with an accumulated time
  else
    m_start = perftime;  // Starting from 0
}

void PerfTimer::Stop()  // Stop timing. Use Start afterwards to continue
{
  if (m_start <= 0) {
    return;  // Was not running
  }
  uint64_t perftime = GetPerfTimeNanoseconds();
  m_start += -perftime;    // Stopped timer keeps elapsed timer ticks as a negative
  if (m_start < m_adjust)  // Do not overflow
    m_start -= m_adjust;   // Adjust for amount time that timer code takes to run
  else
    m_start =
        0;  // Stop must have been called directly after Start (and timer code is executed in zero time!)
}

// Split (get elapsed time, just like a stopwatch, timer does not stop)
double PerfTimer::Elapsed()  // Returns elapsed time in seconds
{
  int64_t time;
  if (m_start > 0) {
    uint64_t perftime = GetPerfTimeNanoseconds();  // Timer is still running
    time = (m_start - perftime) - m_adjust;
  } else {
    time = m_start;  // Timer has stopped
  }
  double t = double(-time);
  return t * ConversionS();
}

double PerfTimer::Elapsedms()  // Returns elapsed time in milliseconds
{
  int64_t time;
  if (m_start > 0) {
    uint64_t perftime = GetPerfTimeNanoseconds();  // Timer is still running
    time = (m_start - perftime) - m_adjust;
  } else {
    time = m_start;  // Timer has stopped
  }
  double t = double(-time);
  return t * ConversionMS();
}

double PerfTimer::Elapsedus()  // Returns elapsed time in microseconds
{
  int64_t time;
  if (m_start > 0) {
    uint64_t perftime = GetPerfTimeNanoseconds();  // Timer is still running
    time = (m_start - perftime) - m_adjust;
  } else {
    time = m_start;  // Timer has stopped
  }
  double t = double(-time);
  return t * ConversionUS();
}
