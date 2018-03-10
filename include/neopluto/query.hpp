#ifndef NEOPLUTO_QUERY_HPP
#define NEOPLUTO_QUERY_HPP

#include <neopluto/date.hpp>

#include <optional>
#include <string>

namespace npl
{

class database;

class query
{
  friend class database;

public:
  auto since(date) -> query&;
  auto until(date) -> query&;

private:
  auto where_clause() const -> std::string;

  std::optional<date> since_, until_;
};

} // namespace npl

#endif // NEOPLUTO_QUERY_HPP
