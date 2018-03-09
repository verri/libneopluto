#ifndef NEOPLUTO_NAIVE_BAYES_HPP
#define NEOPLUTO_NAIVE_BAYES_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace npl
{

struct tag_classifier;

namespace nb
{

auto build_tag_model(
  const std::vector<std::pair<std::size_t, std::vector<std::string>>>& dataset,
  std::unordered_map<std::string, std::size_t> words_,
  std::unordered_map<std::size_t, std::int64_t> label_to_tag_) -> tag_classifier*;

auto destroy_tag_model(tag_classifier*) -> void;

auto classify(const tag_classifier*, const std::vector<std::string>& tokens)
  -> std::int64_t;

} // namespace nb

} // namespace npl

#endif // NEOPLUTO_NAIVE_BAYES_HPP
