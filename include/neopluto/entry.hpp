#ifndef NEOPLUTO_ENTRY_HPP
#define NEOPLUTO_ENTRY_HPP

#include <neopluto/account.hpp>
#include <neopluto/date.hpp>
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

  auto is_transfer() const -> bool;
  auto is_income() const -> bool;
  auto is_expense() const -> bool;

  auto update_description(const char*) -> void;
  auto retrieve_description() const -> std::string;

  auto update_date(date) -> void;
  auto retrieve_date() const -> date;

  auto update_value(std::int64_t) -> void;
  auto retrieve_value() const -> std::int64_t;

  auto update_tag(const std::optional<tag>&) -> void;
  auto retrieve_tag() const -> std::optional<tag>;

  auto update_from_account(const std::optional<account>&) -> void;
  auto retrieve_from_account() const -> std::optional<account>;

  auto update_to_account(const std::optional<account>&) -> void;
  auto retrieve_to_account() const -> std::optional<account>;

  auto erase() -> void;

private:
  entry(std::int64_t, std::shared_ptr<database>);

  std::int64_t id;
  std::shared_ptr<database> db;
};

} // namespace pl

#endif // NEOPLUTO_ENTRY_HPP
