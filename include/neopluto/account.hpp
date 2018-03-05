#ifndef NEOPLUTO_ACCOUNT_HPP
#define NEOPLUTO_ACCOUNT_HPP

#include <memory>
#include <string>

namespace npl
{

class database;

class account
{
  friend class database;

public:
  account() = delete;

  auto update_name(const char* name) -> void;
  auto retrieve_name() const -> std::string;

private:
  account(std::int64_t, std::shared_ptr<database>);

  std::int64_t id;
  std::shared_ptr<database> db;
};

} // namespace pl

#endif // NEOPLUTO_ACCOUNT_HPP
