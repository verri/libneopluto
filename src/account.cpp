#include <neopluto/account.hpp>
#include <neopluto/database.hpp>

#include <cassert>
#include <iomanip>
#include <sqlite3.h>
#include <sstream>

namespace npl
{

auto account::update_name(const char* name) -> void
{
  assert(db);

  std::stringstream ss;
  ss << "UPDATE Account SET name = " << std::quoted(name) << " WHERE id = " << id << ';';

  db->exec_query(ss.str().c_str());
}

auto account::retrieve_name() const -> std::string
{
  assert(db);

  std::stringstream ss;
  ss << "SELECT name FROM Account WHERE id = " << id << ";";

  std::string name;

  db->exec_query(ss.str().c_str(), [&](sqlite3_stmt* stmt) {
    name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    return false;
  });

  if (name.empty())
    throw std::runtime_error{"account was deleted"};

  return name;
}

auto account::erase() -> void
{
  assert(db);

  std::stringstream ss;
  ss << "DELETE FROM Account WHERE id = " << id << ';';

  db->exec_query(ss.str().c_str());
  db = nullptr;
}

account::account(std::int64_t id, std::shared_ptr<database> db) : id{id}, db{db} {}

} // namespace npl
