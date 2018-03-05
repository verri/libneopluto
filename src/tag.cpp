#include <neopluto/database.hpp>
#include <neopluto/tag.hpp>

#include <cassert>
#include <sqlite3.h>
#include <sstream>

namespace npl
{

auto tag::update_name(const char* name) -> void
{
  assert(db);

  std::stringstream ss;
  ss << "UPDATE Tag SET name  = '" << name << "' WHERE id = " << id << ';';
  db->exec_query(ss.str().c_str());
}

auto tag::retrieve_name() const -> std::string
{
  assert(db);

  std::stringstream ss;
  ss << "SELECT name FROM Tag WHERE id = " << id << ";";

  std::string name;

  db->exec_query(ss.str().c_str(), [&](sqlite3_stmt* stmt) {
    name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    return false;
  });

  if (name.empty())
    throw std::runtime_error{"tag was deleted"};

  return name;
}

auto tag::erase() -> void
{
  assert(db);

  std::stringstream ss;
  ss << "DELETE FROM Tag WHERE id = " << id << ';';

  db->exec_query(ss.str().c_str());
  db = nullptr;
}

tag::tag(std::int64_t id, std::shared_ptr<database> db) : id{id}, db{db} {}

} // namespace npl
