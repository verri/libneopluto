#include <neopluto/query.hpp>

#include <sstream>

namespace npl
{

auto query::since(date d) -> query&
{
  since_ = d;
  return *this;
}

auto query::until(date d) -> query&
{
  until_ = d;
  return *this;
}

auto query::where_clause() const -> std::string
{
  std::stringstream ss;

  auto add_restriction = [&ss, empty = true ](auto&&... args) mutable
  {
    if (!empty)
      ss << " AND ";
    (ss << ... << args);
    empty = false;
  };

  if (since_)
    add_restriction("date >= ", since_->to_integer());

  if (until_)
    add_restriction("date <= ", until_->to_integer());

  const auto clause = ss.str();
  return clause.empty() ? clause : "WHERE " + clause;
}

} // namespace npl