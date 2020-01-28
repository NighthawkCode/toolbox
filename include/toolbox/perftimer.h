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

  // Timer resloution
  double Resolution()       // Returns timer resolution in seconds   
  { return m_conversionS;   }

  double Resolutionms()     // Returns timer resolution in milliseconds
  {   return m_conversionMS; }

  double Resolutionus()     // Returns timer resolution in microseconds
  {   return m_conversionUS; }

protected:
  int64_t m_start;
  static int64_t m_adjust;      // Adjustment time it takes to Start and Stop
  static double m_conversionS;  // Conversion factor for seconds.
  static double m_conversionMS; // Conversion factor for milliseconds.
  static double m_conversionUS; // Conversion factor for microseconds.
};
