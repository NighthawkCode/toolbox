#include "toolbox/datetime_str_parser.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

std::optional<double> parse_datetime_str_utc(const char* datetime_str) {
  std::istringstream stream{datetime_str};
  double since_epoch;
  stream >> since_epoch;
  if (!stream.fail() && stream.eof()) {
    return since_epoch;
  } else {
    const char iso_format[] = "%Y-%m-%dT%H:%M:%S";
    std::tm timestamp{};
    stream.clear();
    stream.str(datetime_str);
    stream >> std::get_time(&timestamp, iso_format);
    if (stream.fail()) {
      return {};
    }
    double tm_sec_fraction{0};
    if (!stream.eof()) {
      stream >> tm_sec_fraction;
      if (stream.fail()) {
        return {};
      }
    }
    std::time_t num_seconds = timegm(&timestamp);
    since_epoch = static_cast<double>(num_seconds) + tm_sec_fraction;
    return since_epoch;
  }
}
