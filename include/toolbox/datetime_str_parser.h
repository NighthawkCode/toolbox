#pragma once
#include <optional>

// Convert a date-time string to a timestamp, which is the number of seconds since epoch
// If input is not valid then the result will not be present.
//
// Following inputs are valid:
// (1) the ISO 8601 standard datetime string, of which the format is yyyy-mm-ddTHH-MM-SS.fraction but the time
// part is optional.
//   "2021-06-01T16:05:07.000001"
//   "2021-06-01T16:05:07"
//   "2021-06-01T16:05"
//   "2021-06-01T16"
//   "2021-06-01"
//   are all supported valid input
// (2) a float-point number string, like "1622563507.1"
//
// Examples:
// (1) parse a date time string
//   parse_datetime_str_utc("2021-06-01T16:05:07.000001") -> 1622563507.000001
//   parse_datetime_str_utc("2021-06-01T16:05:07") -> 1622563507
// (2) parse a number string
//   parse_datetime_str_utc("1622563507.000001") -> 1622563507.000001
//
// Usage:
//   std::optional<double> timestamp = parse_datetime_str_utc(...);
//   if (!timestamp.has_value()) {
//     // the input is not valid so we do error handling here
//   } else {
//     // we get a valid timestamp
//   }
std::optional<double> parse_datetime_str_utc(const char* datetime_str);
