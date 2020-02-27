#pragma once 
#include <inttypes.h>

// This works like a stopwatch. You can start, stop, split, reset.  
// Also, measure a scope, store in a container, summarize in a report.
class PerfTimer
{
public:
  PerfTimer();
  ~PerfTimer() { }

  // Start timer
  void Start( bool bReset = false );  

  // Stop timing. Use Start afterwards to continue.
  void Stop();        

  // Split (get elapsed time, just like a stopwatch, timer does not stop)
  double Elapsed();       	// Returns elapsed time in seconds
  double Elapsedms();       // Returns elapsed time in milliseconds
  double Elapsedus();       // Returns elapsed time in microseconds

  // Reset after Stop.
  void Reset();
  
  // Measure time it takes to start and stop.
  void Adjust();        

  // Conversion factors
  double ConversionS() const {   
    return (1.0 / 1e+9); }       // Conversion for nanoseconds to seconds.

  double ConversionMS() const {  
    return (1.0 / 1e+6);  }       // Conversion for nanoseconds to milliseconds.

  double ConversionUS() const {  
    return (1.0 / 1e+3);  }       // Conversion for nanoseconds to microseconds.

protected:
  int64_t m_start;
  static int64_t m_adjust;    // Adjustment time it takes to Start and Stop
};
