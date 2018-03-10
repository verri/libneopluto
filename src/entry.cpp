#include <neopluto/database.hpp>
#include <neopluto/entry.hpp>

#include <cassert>
#include <iomanip>
#include <sqlite3.h>
#include <sstream>

namespace npl
{

auto entry::update_description(const char* desc) -> void
{
  assert(db);

  std::stringstream ss;
  ss << "UPDATE Entry SET description = " << std::quoted(desc) << " WHERE id = " << id
     << ';';

  db->exec_query(ss.str().c_str());
}

auto entry::retrieve_description() const -> std::string
{
  assert(db);

  std::stringstream ss;
  ss << "SELECT description FROM Entry WHERE id = " << id << ";";

  std::string name;

  db->exec_query(ss.str().c_str(), [&](sqlite3_stmt* stmt) {
    name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    return false;
  });

  if (name.empty())
    throw std::runtime_error{"entry was deleted"};

  return name;
}

auto entry::update_tag(const std::optional<tag>& t) -> void
{
  assert(db);
  if (!t) {
    std::stringstream ss;
    ss << "UPDATE Entry SET tag = NULL WHERE id = " << id << ';';
    db->exec_query(ss.str().c_str());
    return;
  }

  assert(t->id > 0);
  assert(t->db == db);

  std::stringstream ss;
  ss << "UPDATE Entry SET tag = " << t->id << " WHERE id = " << id << ';';

  db->exec_query(ss.str().c_str());
}

auto entry::erase() -> void
{
  assert(db);

  std::stringstream ss;
  ss << "DELETE FROM Account WHERE id = " << id << ';';

  db->exec_query(ss.str().c_str());
  db = nullptr;
}

entry::entry(std::int64_t id, std::shared_ptr<database> db) : id{id}, db{db} {}

} // namespace npl
