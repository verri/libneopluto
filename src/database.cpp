#include <neopluto/database.hpp>
#include <neopluto/exception.hpp>

#include <cassert>
#include <cool/defer.hpp>
#include <iomanip>
#include <sqlite3.h>
#include <sstream>

#ifndef NDEBUG
#include <iostream>
#endif // NDEBUG

namespace
{
constexpr char init_db_query[] = R"(
  CREATE TABLE IF NOT EXISTS Account (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE
  );

  CREATE TABLE IF NOT EXISTS Tag (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE
  );

  CREATE TABLE IF NOT EXISTS Entry (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    date INTEGER NOT NULL,
    accountfrom INTEGER DEFAULT NULL,
    accountto INTEGER DEFAULT NULL,
    description TEXT NOT NULL,
    value REAL NOT NULL,
    tag INTEGER DEFAULT NULL,

    FOREIGN KEY (accountfrom) REFERENCES Account(id) ON DELETE SET NULL,
    FOREIGN KEY (accountto) REFERENCES Account(id) ON DELETE SET NULL,
    FOREIGN KEY (tag) REFERENCES Tag(id) ON DELETE SET NULL
  );

  CREATE TABLE IF NOT EXISTS Words (
    id INTEGER PRIMARY KEY,
    word TEXT NOT NULL
  );

  CREATE TABLE IF NOT EXISTS Model (
    id INTEGER PRIMARY KEY,
    model TEXT NOT NULL
  );

  PRAGMA foreign_keys = ON;)";
}

namespace npl
{

auto database::open(const char* filename) -> std::shared_ptr<database>
{
  return std::shared_ptr<database>{new database(filename)};
}

auto database::create_account(const char* name) -> account
{
  assert(db);

  std::stringstream ss;
  ss << "INSERT INTO Account(name) VALUES (" << std::quoted(name) << ");";

  std::int64_t id = -1;
  try {
    exec_query(ss.str().c_str());
    id = sqlite3_last_insert_rowid(db);
  } catch (sqlite_exception& e) {
    if (e.retval == SQLITE_CONSTRAINT_UNIQUE)
      throw std::runtime_error{"account already exists"};
    throw;
  }

  assert(id > 0);
  return account(id, shared_from_this());
}

auto database::retrieve_account(const char* name, bool create) -> account
{
  assert(db);
  std::int64_t id = -1;

  std::stringstream ss;
  ss << "SELECT id FROM Account WHERE name = " << std::quoted(name) << ";";

  exec_query(ss.str().c_str(), [&](sqlite3_stmt* stmt) {
    id = sqlite3_column_int(stmt, 0);
    return false;
  });

  if (!create && id < 0)
    throw std::runtime_error{"account does not exist"};

  if (create && id < 0)
    return create_account(name);

  return account(id, shared_from_this());
}

auto database::retrieve_accounts(const std::function<bool(account)>& callback) -> void
{
  assert(db);
  exec_query("SELECT id FROM Account ORDER BY name;", [&](sqlite3_stmt* stmt) {
    return callback({sqlite3_column_int(stmt, 0), shared_from_this()});
  });
}

auto database::retrieve_accounts(std::size_t max) -> std::vector<account>
{
  assert(db);

  std::vector<account> accounts;
  exec_query("SELECT id FROM Account ORDER BY name;", [&](sqlite3_stmt* stmt) mutable {
    if (accounts.size() >= max)
      return false;
    accounts.push_back({sqlite3_column_int(stmt, 0), shared_from_this()});
    return true;
  });

  return accounts;
}

auto database::create_tag(const char* name) -> tag
{
  assert(db);

  std::stringstream ss;
  ss << "INSERT INTO Tag(name) VALUES (" << std::quoted(name) << ");";

  std::int64_t id = -1;
  try {
    exec_query(ss.str().c_str());
    id = sqlite3_last_insert_rowid(db);
  } catch (sqlite_exception& e) {
    if (e.retval == SQLITE_CONSTRAINT_UNIQUE)
      throw std::runtime_error{"tag already exists"};
    throw;
  }

  assert(id > 0);
  return tag(id, shared_from_this());
}

auto database::retrieve_tag(const char* name, bool create) -> tag
{
  assert(db);
  std::int64_t id = -1;

  std::stringstream ss;
  ss << "SELECT id FROM Tag WHERE name = " << std::quoted(name) << ";";

  exec_query(ss.str().c_str(), [&](sqlite3_stmt* stmt) {
    id = sqlite3_column_int(stmt, 0);
    return false;
  });

  if (!create && id < 0)
    throw std::runtime_error{"tag does not exist"};

  if (create && id < 0)
    return create_tag(name);

  return tag(id, shared_from_this());
}

auto database::retrieve_tags(const std::function<bool(tag)>& callback) -> void
{
  assert(db);
  exec_query("SELECT id FROM Tag ORDER BY name;", [&](sqlite3_stmt* stmt) {
    return callback({sqlite3_column_int(stmt, 0), shared_from_this()});
  });
}

auto database::retrieve_tags(std::size_t max) -> std::vector<tag>
{
  assert(db);

  std::vector<tag> tags;
  exec_query("SELECT id FROM Tag ORDER BY name;", [&](sqlite3_stmt* stmt) mutable {
    if (tags.size() >= max)
      return false;
    tags.push_back({sqlite3_column_int(stmt, 0), shared_from_this()});
    return true;
  });

  return tags;
}

auto database::income(date d, const account& ac, const char* desc, double value) -> entry
{
  assert(db);

  std::stringstream ss;
  ss << "INSERT INTO Entry(date, accountto, description, value) VALUES ("
     << d.to_integer() << ", " << ac.id << ", " << std::quoted(desc) << ", " << value
     << ");";

  std::int64_t id = -1;
  exec_query(ss.str().c_str());
  id = sqlite3_last_insert_rowid(db);

  assert(id > 0);
  return entry(id, shared_from_this());
}

auto database::expense(date d, const account& ac, const char* desc, double value) -> entry
{
  assert(db);

  std::stringstream ss;
  ss << "INSERT INTO Entry(date, accountfrom, description, value) VALUES ("
     << d.to_integer() << ", " << ac.id << ", " << std::quoted(desc) << ", " << value
     << ");";

  std::int64_t id = -1;
  exec_query(ss.str().c_str());
  id = sqlite3_last_insert_rowid(db);

  assert(id > 0);
  return entry(id, shared_from_this());
}

auto database::transfer(date d, const account& from, const account& to, const char* desc,
                        double value) -> entry
{
  assert(db);

  std::stringstream ss;
  ss << "INSERT INTO Entry(date, accountfrom, accountto, description, value) VALUES ("
     << d.to_integer() << ", " << from.id << ", " << to.id << ", " << std::quoted(desc)
     << ", " << value << ");";

  std::int64_t id = -1;
  exec_query(ss.str().c_str());
  id = sqlite3_last_insert_rowid(db);

  assert(id > 0);
  return entry(id, shared_from_this());
}

database::database(const char* filename)
{
  auto ok = false;
  const auto retval = sqlite3_open(filename, &db);

  if (retval != SQLITE_OK)
    throw sqlite_exception{"sqlite3_open", retval};

  COOL_DEFER({
    if (!ok) {
      sqlite3_close(db);
      db = nullptr;
    }
  });

  sqlite3_extended_result_codes(db, true);
  exec_query(init_db_query);

  ok = true;
}

database::~database()
{
  if (!db)
    return;

  try {
    const auto retval = sqlite3_close(db);
    if (retval != SQLITE_OK)
      throw sqlite_exception("sqlite3_close", retval);
  } catch (sqlite_exception& exc) {
#ifndef NDEBUG
    std::clog << "[neopluto] error while calling database::~database: " << exc.routine
              << " (" << exc.retval
              << "): " << (exc.msg == nullptr ? "[no message]" : exc.msg) << '\n';
    std::abort();
#endif
  }
}

auto database::exec_query(const char* query) -> void
{
  int retval;
  sqlite3_stmt* stmt;

  while (query[0] != '\0') {
#ifndef NDEBUG
    std::clog << ">\t";
    char c;
    int i = 0;
    while ((c = query[i++]) != ';')
      std::clog << c;
    std::clog << ';' << std::endl;
#endif // NDEBUG

    retval = sqlite3_prepare_v2(db, query, -1, &stmt, &query);
    if (retval != SQLITE_OK)
      throw sqlite_exception{"sqlite3_prepare_v2", retval};

    {
      // Ensures to finalize the statement.
      COOL_DEFER({ retval = sqlite3_finalize(stmt); });

      retval = sqlite3_step(stmt);
      if (retval != SQLITE_DONE)
        throw sqlite_exception{"sqlite3_step", retval,
                               retval == SQLITE_ERROR ? sqlite3_errmsg(db) : nullptr};
    }

    if (retval /* sqlite3_finalize */ != SQLITE_OK)
      throw sqlite_exception{"sqlite3_finalize", retval};
  }
}

auto database::exec_query(const char* query, std::function<bool(sqlite3_stmt*)> callback)
  -> void
{
  int retval;
  sqlite3_stmt* stmt;

#ifndef NDEBUG
  std::clog << ">\t" << query << std::endl;
#endif // NDEBUG

  retval = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
  if (retval != SQLITE_OK)
    throw sqlite_exception{"sqlite3_prepare_v2", retval};

  {
    // Ensures to finalize the statement.
    COOL_DEFER({ retval = sqlite3_finalize(stmt); });

    while ((retval = sqlite3_step(stmt)) == SQLITE_ROW) {
      if (callback(stmt))
        continue;
      retval = SQLITE_DONE;
      break;
    }

    if (retval != SQLITE_DONE)
      throw sqlite_exception{"sqlite3_step", retval,
                             retval == SQLITE_ERROR ? sqlite3_errmsg(db) : nullptr};
  }

  if (retval /* sqlite3_finalize */ != SQLITE_OK)
    throw sqlite_exception{"sqlite3_finalize", retval};
}

} // namespace npl
