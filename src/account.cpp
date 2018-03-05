#include <neopluto/account.hpp>
#include <neopluto/database.hpp>

#include <cassert>
#include <sqlite3.h>
#include <sstream>

namespace npl
{

auto account::update_name(const char* name) -> void
{

  std::stringstream ss;
  ss << "UPDATE Account SET name  = '" << name << "' WHERE id = " << id << ';';

  db->exec_query(ss.str().c_str());
}

auto account::retrieve_name() const -> std::string
{

  std::stringstream ss;
  ss << "SELECT id, name FROM Account WHERE id = " << id << ";";

  std::string name;

  db->exec_query(ss.str().c_str(), [&](sqlite3_stmt* stmt) {
    name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    return false;
  });

  if (name.empty())
    throw std::runtime_error{"account was deleted"};

  return name;
}

account::account(std::int64_t id, std::shared_ptr<database> db) : id{id}, db{db} {}

} // namespace npl
