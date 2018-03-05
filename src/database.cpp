#include <neopluto/database.hpp>
#include <neopluto/exception.hpp>

#include <cassert>
#include <cool/defer.hpp>
#include <functional>
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
    id INTEGER PRIMARY KEY,
    date INTEGER NOT NULL,
    accountfrom INTEGER DEFAULT NULL,
    accountto INTEGER DEFAULT NULL,
    description TEXT NOT NULL,
    tag INTEGER DEFAULT NULL,
    value REAL NOT NULL,

    FOREIGN KEY (accountfrom) REFERENCES Account(id) ON DELETE SET NULL,
    FOREIGN KEY (accountto) REFERENCES Account(id) ON DELETE SET NULL,
    FOREIGN KEY (tag) REFERENCES Tag(id) ON DELETE SET NULL
  );

  PRAGMA foreign_keys = ON;
)";
}

namespace npl
{

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

auto database::income(date, const account&, const char*, const tag&, double) -> entry
{
  return {};
}

auto database::expense(date, const account&, const char*, const tag&, double) -> entry
{
  return {};
}

} // namespace npl
