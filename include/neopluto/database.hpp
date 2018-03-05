#ifndef NEOPLUTO_DATASET_HPP
#define NEOPLUTO_DATASET_HPP

#include <memory>

extern "C" {
typedef struct sqlite3 sqlite3;
}

namespace npl
{

class database
{
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

  auto exec_query(const char*) -> void;

private:
  sqlite3* db = nullptr;
};

} // namespace npl

#endif // NEOPLUTO_DATASET_HPP
