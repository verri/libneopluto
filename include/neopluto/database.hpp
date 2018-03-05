#ifndef NEOPLUTO_DATASET_HPP
#define NEOPLUTO_DATASET_HPP

#include <neopluto/account.hpp>
#include <neopluto/date.hpp>
#include <neopluto/entry.hpp>
#include <neopluto/tag.hpp>

extern "C" {
typedef struct sqlite3 sqlite3;
}

namespace npl
{

class database
{
  friend class tag;
  friend class account;
  friend class entry;

public:
  database() = default;

  database(const char*);
  ~database();

  database(database&&);
  auto operator=(database &&) -> database&;

  database(const database&) = delete;
  auto operator=(const database&) -> database& = delete;

  auto open(const char*) -> void;
  auto close() -> void;

  auto income(date, const account&, const char*, const tag&, double) -> entry;
  auto expense(date, const account&, const char*, const tag&, double) -> entry;
  auto transfer(date, const account&, const account&, const char*, const tag&, double)
    -> entry;

protected:
  auto exec_query(const char*) -> void;

  sqlite3* db = nullptr;
};

} // namespace npl

#endif // NEOPLUTO_DATASET_HPP
