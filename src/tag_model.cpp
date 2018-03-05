#include <neopluto/database.hpp>

#include <sqlite3.h>
#include <sstream>
#include <string>
#include <unordered_map>

#include <boost/locale.hpp>

namespace npl
{

auto database::update_tag_model() -> void
{
  assert(db);

  std::unordered_map<std::string, std::int64_t> words;
  // std::int64_t current = 0;

  std::locale locale("");

  // Get all words
  exec_query("SELECT description FROM Entry WHERE tag IS NOT NULL;",
             [&](sqlite3_stmt* stmt) {
               const auto description = boost::locale::to_upper(
                 reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), locale);

               namespace bb = boost::locale::boundary;
               bb::ssegment_index map(bb::word, description.begin(), description.end());
               map.rule(bb::word_letter);

               for (auto it = map.begin(), e = map.end(); it != e; ++it) {
                 std::clog << "detected word: " << *it << std::endl;
               }

               return true;
             });
}

} // namespace npl
