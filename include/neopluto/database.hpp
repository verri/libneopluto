#ifndef NEOPLUTO_DATASET_HPP
#define NEOPLUTO_DATASET_HPP

#include <neopluto/account.hpp>
#include <neopluto/date.hpp>
#include <neopluto/entry.hpp>
#include <neopluto/tag.hpp>

#include <functional>
#include <memory>
#include <optional>

extern "C" {
typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;
}

namespace npl
{

class database : public std::enable_shared_from_this<database>
{
  friend class tag;
  friend class account;
  friend class entry;

public:
  static auto open(const char*) -> std::shared_ptr<database>;
  ~database();

  auto create_account(const char* name) -> account;
  auto retrieve_account(const char* name, bool create = false) -> account;

  auto create_tag(const char* name) -> tag;
  auto retrieve_tag(const char* name, bool create = false) -> tag;

  auto income(date, const account&, const char*, double) -> entry;

  auto expense(date, const account&, const char*, double) -> entry;

  auto transfer(date, const account&, const account&, const char*, double) -> entry;

  auto update_tag_model() -> void;
  auto clear_tag_model() -> void;
  auto suggest_tag(const char*) -> std::optional<tag>;

private:
  database() = delete;
  database(const char*);

  database(database&&) = delete;
  auto operator=(database &&) -> database& = delete;

  database(const database&) = delete;
  auto operator=(const database&) -> database& = delete;

protected:
  auto exec_query(const char*) -> void;
  auto exec_query(const char*, std::function<bool(sqlite3_stmt*)>) -> void;

  sqlite3* db = nullptr;
  void* tag_model = nullptr;
};

} // namespace npl

#endif // NEOPLUTO_DATASET_HPP
