#include <neopluto/date.hpp>

namespace npl
{

auto date::to_integer() const -> std::int64_t
{
  return year * 10000l + month * 100l + day;
}

auto date::from_integer(std::int64_t value) -> date
{
  date d;
  d.year = value / 10000l;
  d.month = (value - d.year * 10000l) / 100l;
  d.day = (value - d.year * 10000l - d.month * 100l);
  return d;
}

} // namespace npl
