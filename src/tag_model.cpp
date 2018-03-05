#include <neopluto/database.hpp>

#include <sqlite3.h>
#include <sstream>
#include <string>
#include <unordered_map>

#include <boost/locale.hpp>

#ifndef NDEBUG
#include <iostream>
#endif

namespace npl
{

auto database::update_tag_model() -> void
{
  assert(db);

  std::unordered_map<std::string, std::int64_t> words;
  std::int64_t current = 0;
  std::size_t nentries = 0;

  boost::locale::generator gen;
  std::locale locale = gen("");

#ifndef NDEBUG
  std::clog << "Using locale: " << locale.name() << std::endl;
#endif

  // Get all words
  constexpr char query[] = "SELECT description FROM Entry;";
  exec_query(query, [&](sqlite3_stmt* stmt) {
    ++nentries;

    const auto description = boost::locale::to_upper(
      reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), locale);
#ifndef NDEBUG
    std::clog << "Entry description: " << description << std::endl;
#endif

    namespace bb = boost::locale::boundary;
    bb::ssegment_index map(bb::word, description.begin(), description.end(), locale);
    map.rule(bb::word_letter);

    for (auto it = map.begin(), e = map.end(); it != e; ++it) {
      if (const auto[word, inserted] = words.try_emplace(*it, current); inserted) {
#ifndef NDEBUG
        std::clog << "detected new word: " << current << ": " << *it << std::endl;
#endif
        ++current;
      }
    }

    return true;
  });
}

} // namespace npl
