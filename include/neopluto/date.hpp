#ifndef NEOPLUTO_DATE_HPP
#define NEOPLUTO_DATE_HPP

#include <cstdint>

namespace npl
{

struct date {
  unsigned year;
  unsigned month;
  unsigned day;

  auto to_integer() const -> std::int64_t;
  static auto from_integer(std::int64_t) -> date;
};

} // namespace pl

#endif // NEOPLUTO_DATE_HPP
