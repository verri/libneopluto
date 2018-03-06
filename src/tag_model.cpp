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
  tag_classifier(const jules::vector<std::size_t>& labels, const jules::matrix<>& occ,
                 std::unordered_map<std::string, std::size_t> words_,
                 std::unordered_map<std::size_t, std::int64_t> label_to_tag_)
    : nwords{words_.size()}, nlabels{label_to_tag_.size()}, words(std::move(words_)),
      label_to_tag(std::move(label_to_tag_)), label_prob(jules::uninitialized, nlabels),
      word_present_prob(jules::uninitialized, nwords, nlabels)
  {
    assert(occ.column_count() == nwords);
    assert(occ.row_count() == labels.size());

    using jules::indices;

    for (const auto i : indices(label_prob.size()))
      label_prob[i] = freq(labels == i);
#ifndef NDEBUG
    std::clog << "class probs: " << label_prob << std::endl;
#endif

    for (const auto l : indices(nlabels)) {
      const auto ix = which(labels == l);
      for (const auto w : indices(nwords)) {
        word_present_prob[w][l] = (1.0 + count(occ[ix][w])) / (ix.size() + nwords);
      }
    }

#ifndef NDEBUG
    std::clog << "P(W|L): " << word_present_prob << std::endl;
#endif
  }

  auto classify(const char* desc) const -> std::int64_t
  {
    using jules::indices;
    using jules::log;
    using std::log;

    std::locale locale = boost::locale::generator{}("");
    const auto description = boost::locale::to_upper(desc, locale);

    namespace bb = boost::locale::boundary;
    bb::ssegment_index map(bb::word, description.begin(), description.end(), locale);
    map.rule(bb::word_letter);

    jules::vector<bool> sample(false, nwords);
    for (auto it = map.begin(), e = map.end(); it != e; ++it)
      sample[words.at(*it)] = true;

    jules::vector<> probs = log(label_prob);
    for (const auto l : indices(nlabels))
      for (const auto w : indices(nwords))
        probs[l] +=
          log(sample[w] ? word_present_prob[w][l] : 1.0 - word_present_prob[w][l]);

#ifndef NDEBUG
    std::clog << "\nClassifying: " << desc << std::endl;
    std::clog << "sample: " << sample << std::endl;
    std::clog << "probs: " << as_vector(probs / sum(probs)) << std::endl;
#endif

    return label_to_tag.at(which_max(probs));
  }

private:
  std::size_t nwords, nlabels;

  std::unordered_map<std::string, std::size_t> words;
  std::unordered_map<std::size_t, std::int64_t> label_to_tag;

  jules::vector<> label_prob;
  jules::matrix<> word_present_prob;
};
}

namespace npl
{

auto database::update_tag_model() -> void
{
  assert(db);

  if (tag_model)
    clear_tag_model();

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

  // Frequency matrix
  jules::matrix<bool> occ(false, dataset.size(), words.size());
  jules::vector<std::size_t> labels(jules::uninitialized, dataset.size());

  std::size_t current = 0;
  for (const auto & [ label, desc ] : dataset) {
    labels[current] = label;
    for (const auto& token : desc)
      occ[current][words[token]] = true;
    ++current;
  }

#ifndef NDEBUG
  std::clog << "labels: " << labels << std::endl;
  std::clog << "freq: " << occ << std::endl;
#endif

  tag_classifier* const classifier =
    new tag_classifier(std::move(labels), occ, std::move(words), std::move(label_to_tag));
  tag_model = classifier;
}

auto database::clear_tag_model() -> void
{
  const auto classifier = reinterpret_cast<tag_classifier*>(tag_model);
  if (classifier)
    delete classifier;
}

auto database::suggest_tag(const char* desc) -> std::optional<tag>
{
  const auto classifier = reinterpret_cast<const tag_classifier*>(tag_model);
  if (!classifier)
    return std::nullopt;

  return tag(classifier->classify(desc), shared_from_this());
}

} // namespace npl
