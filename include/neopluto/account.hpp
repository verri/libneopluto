#ifndef NEOPLUTO_ACCOUNT_HPP
#define NEOPLUTO_ACCOUNT_HPP

#include <memory>
#include <string>

namespace npl
{

class database;
class entry;
class query;

class account
{
  friend class database;
  friend class entry;
  friend class query;

public:
  account() = delete;

  auto update_name(const char*) -> void;
  auto retrieve_name() const -> std::string;
  auto erase() -> void;

private:
  account(std::int64_t, std::shared_ptr<database>);

  std::int64_t id;
  std::shared_ptr<database> db;
};

} // namespace pl

#endif // NEOPLUTO_ACCOUNT_HPP
