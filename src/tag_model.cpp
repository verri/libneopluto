#include <neopluto/database.hpp>

#include <sqlite3.h>
#include <sstream>
#include <string>
#include <unordered_map>

#include <boost/locale.hpp>
#include <jules/array/array.hpp>

#ifndef NDEBUG
#include <iostream>
#endif

namespace
{
class tag_classifier
{
public:

private:
  jules::matrix<> freq;
};

}
namespace npl
{

auto database::update_tag_model() -> void
{
  assert(db);

  if (tag_model)
    clear_tag_model();

  tag_classifier* const classifier = new tag_classifier();
  tag_model = classifier;

  std::unordered_map<std::string, std::int64_t> words;
  std::int64_t current = 0;
  std::size_t nentries = 0;

  boost::locale::generator gen;
  std::locale locale = gen("");

#ifndef NDEBUG
  std::clog << "Using locale: " << locale.name() << std::endl;
#endif

  // Get all words
  constexpr char query[] = "SELECT description FROM Entry WHERE tag IS NOT NULL;";
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

auto database::clear_tag_model() -> void
{
  const auto classifier = reinterpret_cast<tag_classifier*>(tag_model);
  if (classifier)
    delete classifier;
}

auto database::suggest_tag(const char*) const -> std::optional<tag>
{
  const auto classifier = reinterpret_cast<tag_classifier*>(tag_model);
  if (!classifier)
    return std::nullopt;

  return {};
}

} // namespace npl
