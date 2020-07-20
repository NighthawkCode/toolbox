#include "time_format.h"

#include <time.h>

#include <cmath>

std::string epoch_to_sql_time(double t) {
  double fracpart, intpart;
  fracpart = std::modf(t, &intpart);
  time_t epoch = (time_t)intpart;
  struct tm* tm = gmtime(&epoch);
  char outstr[100];
  strftime(outstr, sizeof(outstr), "%F %T", tm);
  std::string res = outstr;
  res += "." + std::to_string(int(fracpart * 10000));
  return res;
}
