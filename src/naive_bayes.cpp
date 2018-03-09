#include <neopluto/naive_bayes.hpp>

#include <jules/array/array.hpp>

#ifndef NDEBUG
#include <iostream>
#endif

namespace npl
{

struct tag_classifier {
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

  auto classify(const std::vector<std::string>& tokens) const -> std::int64_t
  {
    using jules::indices;
    using jules::log;
    using std::log;

    jules::vector<bool> sample(false, nwords);
    for (const auto& token : tokens)
      sample[words.at(token)] = true;

    jules::vector<> probs = log(label_prob);
    for (const auto l : indices(nlabels))
      for (const auto w : indices(nwords))
        probs[l] +=
          log(sample[w] ? word_present_prob[w][l] : 1.0 - word_present_prob[w][l]);

#ifndef NDEBUG
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

namespace nb
{

auto build_tag_model(
  const std::vector<std::pair<std::size_t, std::vector<std::string>>>& dataset,
  std::unordered_map<std::string, std::size_t> words,
  std::unordered_map<std::size_t, std::int64_t> label_to_tag) -> tag_classifier*
{
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

  return new tag_classifier(std::move(labels), occ, std::move(words),
                            std::move(label_to_tag));
}

auto destroy_tag_model(tag_classifier* classifier) -> void { delete classifier; }

auto classify(const tag_classifier* classifier, const std::vector<std::string>& tokens)
  -> std::int64_t
{
  return classifier->classify(tokens);
}

} // namespace nb

} // namespace npl
