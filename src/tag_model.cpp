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
  std::vector<std::pair<std::int64_t, std::vector<std::string>>> dataset;

  std::int64_t current = 0;

  boost::locale::generator gen;
  std::locale locale = gen("");

#ifndef NDEBUG
  std::clog << "Using locale: " << locale.name() << std::endl;
#endif

  // Get all words.
  constexpr char query[] = "SELECT description, tag FROM Entry WHERE tag IS NOT NULL;";

  exec_query(query, [&](sqlite3_stmt* stmt) {
    const auto description = boost::locale::to_upper(
      reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), locale);

    const auto tag = sqlite3_column_int(stmt, 1);

#ifndef NDEBUG
    std::clog << "Entry description: " << description << std::endl;
#endif

    namespace bb = boost::locale::boundary;
    bb::ssegment_index map(bb::word, description.begin(), description.end(), locale);
    map.rule(bb::word_letter);

    dataset.push_back({tag, {map.begin(), map.end()}});

    for (const auto& word : dataset.back().second) {
      if (const auto[it, inserted] = words.try_emplace(word, current); inserted) {
#ifndef NDEBUG
        std::clog << "detected new word: " << it->second << ": " << it->first
                  << std::endl;
#endif
        ++current;
      }
    }

    return true;
  });

  // Frequency matrix
  jules::matrix<> freq(0.0, dataset.size(), words.size());
  jules::vector<std::int64_t> tags(jules::uninitialized, dataset.size());

  current = 0;
  for (const auto & [ tag, desc ] : dataset) {
    tags[current] = tag;
    for (const auto& token : desc)
      freq[current][words[token]] += 1.0;
    ++current;
  }

#ifndef NDEBUG
  std::clog << "tags: " << tags << std::endl;
  std::clog << "freq: " << freq << std::endl;
#endif
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
