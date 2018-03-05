#include "neopluto/database.hpp"

#include "neopluto/exception.hpp"

#include <cassert>
#include <cool/defer.hpp>
#include <sqlite3.h>

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

  CREATE TABLE IF NOT EXISTS Tracking (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    date INTEGER NOT NULL,
    account INTEGER NOT NULL,
    description TEXT NOT NULL,
    tag INTEGER DEFAULT NULL,
    quantity REAL NOT NULL,
    value REAL NOT NULL,

    CHECK (quantity > 0),
    FOREIGN KEY (account) REFERENCES Account(id) ON DELETE CASCADE,
    FOREIGN KEY (tag) REFERENCES Tag(id) ON DELETE SET NULL
  );

  PRAGMA foreign_keys = ON;
)";
}

namespace npl
{

database::database(const char* filename) { open(filename); }

database::~database()
{
  try {
    close();
  } catch (sqlite_exception& exc) {
#ifndef NDEBUG
    std::clog << "[neopluto] error while calling database::~database: " << exc.routine
              << " (" << exc.retval
              << "): " << (exc.msg == nullptr ? "[no message]" : exc.msg) << '\n';
    std::abort();
#endif
  }
}

database::database(database&& other)
{
  assert(db == nullptr);
  std::swap(db, other.db);
}

auto database::operator=(database&& other) -> database&
{
  std::swap(db, other.db);
  return *this;
}

auto database::open(const char* filename) -> void
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

auto database::close() -> void
{
  if (!db)
    return;

  const auto retval = sqlite3_close(db);
  if (retval != SQLITE_OK)
    throw sqlite_exception("sqlite3_close", retval);

  db = nullptr;
}

} // namespace npl
