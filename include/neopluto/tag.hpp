#ifndef NEOPLUTO_TAG_HPP
#define NEOPLUTO_TAG_HPP

#include <memory>
#include <string>

namespace npl
{

class database;

class tag
{
public:
  tag() = delete;

private:
  long id;
  std::string name;
  std::shared_ptr<database> db;
};

} // namespace pl

#endif // NEOPLUTO_TAG_HPP
