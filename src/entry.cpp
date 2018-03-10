#include <neopluto/database.hpp>
#include <neopluto/entry.hpp>

#include <cassert>
#include <iomanip>
#include <sqlite3.h>
#include <sstream>

namespace npl
{

auto entry::is_transfer() const -> bool
{
  assert(db);

  std::stringstream ss;
  ss << "SELECT id FROM Entry WHERE id = " << id
     << " AND accountfrom IS NOT NULL AND accountto IS NOT NULL;";

  bool result = false;
  db->exec_query(ss.str().c_str(), [&result](sqlite3_stmt*) {
    result = true;
    return false;
  });

  return result;
}

auto entry::is_income() const -> bool
{
  assert(db);

  std::stringstream ss;
  ss << "SELECT id FROM Entry WHERE id = " << id
     << " AND accountfrom IS NULL AND accountto IS NOT NULL;";

  bool result = false;
  db->exec_query(ss.str().c_str(), [&result](sqlite3_stmt*) {
    result = true;
    return false;
  });

  return result;
}

auto entry::is_expense() const -> bool
{
  assert(db);

  std::stringstream ss;
  ss << "SELECT id FROM Entry WHERE id = " << id
     << " AND accountfrom IS NOT NULL AND accountto IS NULL;";

  bool result = false;
  db->exec_query(ss.str().c_str(), [&result](sqlite3_stmt*) {
    result = true;
    return false;
  });

  return result;
}

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

auto entry::update_date(const date d) -> void
{
  assert(db);

  std::stringstream ss;
  ss << "UPDATE Entry SET date = " << d.to_integer() << " WHERE id = " << id << ';';

  db->exec_query(ss.str().c_str());
}

auto entry::retrieve_date() const -> date
{
  assert(db);

  std::stringstream ss;
  ss << "SELECT date FROM Entry WHERE id = " << id << ";";

  date d;
  db->exec_query(ss.str().c_str(), [&d](sqlite3_stmt* stmt) {
    d = date::from_integer(sqlite3_column_int(stmt, 0));
    return false;
  });

  return d;
}

auto entry::update_value(std::int64_t value) -> void
{
  assert(db);

  std::stringstream ss;
  ss << "UPDATE Entry SET value = " << value << " WHERE id = " << id << ';';

  db->exec_query(ss.str().c_str());
}

auto entry::retrieve_value() const -> std::int64_t
{
  assert(db);

  std::stringstream ss;
  ss << "SELECT value FROM Entry WHERE id = " << id << ";";

  std::int64_t value;
  db->exec_query(ss.str().c_str(), [&value](sqlite3_stmt* stmt) {
    value = sqlite3_column_int(stmt, 0);
    return false;
  });

  return value;
}

auto entry::update_tag(const std::optional<tag>& t) -> void
{
  assert(db);
  assert(!db || t->id > 0);
  assert(!db || t->db == db);

  std::stringstream ss;
  ss << "UPDATE Entry SET tag = ";

  if (t)
    ss << t->id;
  else
    ss << "NULL";

  ss << " WHERE id = " << id << ';';

  db->exec_query(ss.str().c_str());
}

auto entry::retrieve_tag() const -> std::optional<tag>
{
  assert(db);

  std::stringstream ss;
  ss << "SELECT tag FROM Entry WHERE id = " << id << ";";

  std::int64_t id = -1;
  db->exec_query(ss.str().c_str(), [&](sqlite3_stmt* stmt) {
    id = sqlite3_column_int(stmt, 0);
    return false;
  });

  if (id == -1)
    return std::nullopt;

  return tag{id, db};
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
