#ifndef NEOPLUTO_DATASET_HPP
#define NEOPLUTO_DATASET_HPP

#include <neopluto/account.hpp>
#include <neopluto/date.hpp>
#include <neopluto/entry.hpp>
#include <neopluto/tag.hpp>

#include <functional>
#include <memory>

extern "C" {
typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;
}

namespace npl
{

class database
{
  friend class tag;
  friend class account;
  friend class entry;

  static auto open(const char*) -> std::shared_ptr<database>;

  auto get_account(const char* name) -> account;

  auto get_tag(const char* name) -> tag;

  auto income(date, const account&, const char*, const tag&, double) -> entry;

  auto expense(date, const account&, const char*, const tag&, double) -> entry;

  auto transfer(date, const account&, const account&, const char*, const tag&, double)
    -> entry;

private:
  database(const char*);
  ~database();

  database(database&&) = delete;
  auto operator=(database &&) -> database& = delete;

  database(const database&) = delete;
  auto operator=(const database&) -> database& = delete;

protected:
  auto exec_query(const char*) -> void;
  auto exec_query(const char*, std::function<bool(sqlite3_stmt*)>) -> void;

  sqlite3* db = nullptr;
};

} // namespace npl

#endif // NEOPLUTO_DATASET_HPP
