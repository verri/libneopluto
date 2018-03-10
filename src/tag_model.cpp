#include <neopluto/database.hpp>
#include <neopluto/naive_bayes.hpp>

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

  std::unordered_map<std::string, std::size_t> words;
  std::unordered_map<std::size_t, std::int64_t> label_to_tag;
  std::unordered_map<std::int64_t, std::size_t> tag_to_label;

  std::vector<std::pair<std::size_t, std::vector<std::string>>> dataset;

  std::size_t current_word = 0;
  std::size_t current_label = 0;

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

    if (const auto[it, inserted] = tag_to_label.try_emplace(tag, current_label);
        inserted) {
#ifndef NDEBUG
      std::clog << "detected new tag: " << it->second << ": " << it->first << std::endl;
#endif
      label_to_tag.emplace(current_label, tag);
      ++current_label;
    }

#ifndef NDEBUG
    std::clog << "Entry description: " << description << std::endl;
#endif

    namespace bb = boost::locale::boundary;
    bb::ssegment_index map(bb::word, description.begin(), description.end(), locale);
    map.rule(bb::word_letter);

    dataset.push_back({tag_to_label.at(tag), {map.begin(), map.end()}});

    for (const auto& word : dataset.back().second) {
      if (const auto[it, inserted] = words.try_emplace(word, current_word); inserted) {
#ifndef NDEBUG
        std::clog << "detected new word: " << it->second << ": " << it->first
                  << std::endl;
#endif
        ++current_word;
      }
    }

    return true;
  });

  classifier = std::unique_ptr<tag_classifier, void (*)(tag_classifier*)>(
    nb::build_tag_model(dataset, std::move(words), std::move(label_to_tag)),
    &nb::destroy_tag_model);
}

auto database::clear_tag_model() -> void { classifier = nullptr; }

auto database::suggest_tag(const char* desc) -> std::optional<tag>
{
  if (!classifier)
    return std::nullopt;

#ifndef NDEBUG
  std::clog << "\nClassifying: " << desc << std::endl;
#endif

  std::locale locale = boost::locale::generator{}("");
  const auto description = boost::locale::to_upper(desc, locale);

  namespace bb = boost::locale::boundary;
  bb::ssegment_index map(bb::word, description.begin(), description.end(), locale);
  map.rule(bb::word_letter);

  return tag(nb::classify(classifier.get(), {map.begin(), map.end()}),
             shared_from_this());
}

} // namespace npl
