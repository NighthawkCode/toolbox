#include "toolbox/tictoc.h"

#include <time.h> 
#include <sys/time.h>   
#include <sys/resource.h> 
#include <stdio.h>

namespace timing
{
  double tic()
  {
    struct timespec tv;
    clock_gettime(CLOCK_REALTIME, &tv);
    return tv.tv_sec + (double)tv.tv_nsec/1e9;
  }

  double toc( double t0 )
  {
    return tic() - t0;
  }
}
