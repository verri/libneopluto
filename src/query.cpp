#include <neopluto/query.hpp>

#include <sstream>

namespace npl
{

auto query::since(std::optional<date> d) -> query&
{
  since_ = std::move(d);
  return *this;
}

auto query::until(std::optional<date> d) -> query&
{
  until_ = std::move(d);
  return *this;
}

auto query::from(std::optional<account> acc) -> query&
{
  from_ = std::move(acc);
  return *this;
}

auto query::to(std::optional<account> acc) -> query&
{
  to_ = std::move(acc);
  return *this;
}

auto query::has_tag(std::optional<tag> t) -> query&
{
  tag_ = std::move(t);
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
