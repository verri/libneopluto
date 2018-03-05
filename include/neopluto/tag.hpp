#ifndef NEOPLUTO_TAG_HPP
#define NEOPLUTO_TAG_HPP

#include <memory>
#include <string>

namespace npl
{

class database;

class tag
{
  friend class database;

public:
  tag() = delete;

  auto update_name(const char*) -> void;
  auto retrieve_name() const -> std::string;
  auto erase() -> void;

private:
  tag(std::int64_t, std::shared_ptr<database>);

  std::int64_t id;
  std::shared_ptr<database> db;
};

} // namespace pl

#endif // NEOPLUTO_TAG_HPP
