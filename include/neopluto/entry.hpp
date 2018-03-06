#ifndef NEOPLUTO_ENTRY_HPP
#define NEOPLUTO_ENTRY_HPP

#include <neopluto/tag.hpp>

#include <memory>
#include <optional>
#include <string>

namespace npl
{

class database;

class entry
{
  friend class database;

public:
  entry() = delete;

  auto is_transfer() -> bool;
  auto is_income() -> bool;
  auto is_expense() -> bool;

  auto update_description(const char*) -> void;
  auto retrieve_description() const -> std::string;

  auto update_tag(const tag&) -> void;
  auto update_tag(const std::optional<tag>&) -> void;
  auto has_tag() const -> bool;
  auto retrieve_tag() const -> std::optional<tag>;
  auto erase_tag() -> void;

  auto erase() -> void;

private:
  entry(std::int64_t, std::shared_ptr<database>);

  std::int64_t id;
  std::shared_ptr<database> db;
};

} // namespace pl

#endif // NEOPLUTO_ENTRY_HPP
