#ifndef NEOPLUTO_ACCOUNT_HPP
#define NEOPLUTO_ACCOUNT_HPP

#include <memory>
#include <string>

namespace npl
{

class database;

class account
{
public:
  account() = delete;

private:
  long id;
  std::string name;
  std::shared_ptr<database> db;
};

} // namespace pl

#endif // NEOPLUTO_ACCOUNT_HPP
