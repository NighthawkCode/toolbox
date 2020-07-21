#include "toolbox/tictoc.h"

#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <time.h>

namespace timing {

double tic() {
  struct timespec tv;
  clock_gettime(CLOCK_REALTIME, &tv);
  return double(tv.tv_sec) + double(tv.tv_nsec) / 1e9;
}

double toc(double t0) { return tic() - t0; }

}  // namespace timing
