#include "toolbox/tictoc.h"

#include <sys/time.h>
#include <stdio.h>

namespace time
{
  double tic()
  {
    timeval tv;
    gettimeofday( &tv, NULL );
    return tv.tv_sec + (double)tv.tv_usec/1e6;
  }

  double toc( double t0 )
  {
    return tic() - t0;
  }
}
