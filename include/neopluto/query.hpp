#ifndef NEOPLUTO_QUERY_HPP
#define NEOPLUTO_QUERY_HPP

#include <neopluto/account.hpp>
#include <neopluto/date.hpp>
#include <neopluto/tag.hpp>

#include <optional>
#include <string>

namespace npl
{

class database;

class query
{
  friend class database;

public:
  auto since(std::optional<date>) -> query&;
  auto until(std::optional<date>) -> query&;

  auto from(std::optional<account>) -> query&;
  auto to(std::optional<account>) -> query&;

  auto has_tag(std::optional<tag>) -> query&;

private:
  auto where_clause() const -> std::string;

  std::optional<date> since_, until_;
  std::optional<account> from_, to_;
  std::optional<tag> tag_;
};

} // namespace npl

#endif // NEOPLUTO_QUERY_HPP
