// http://adventofcode.com/2024/day/19

#include "advent_of_code/2024/day19/day19.h"

#include "absl/functional/function_ref.h"
#include "absl/log/log.h"
#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

namespace {

static int CharIdx(char c) {
  static std::array<int, 128> kCharIdxArray = []() {
    std::array<int, 128> ret;
    for (int i = 0; i < 128; ++i) ret[i] = -1;
    ret['w'] = 0;
    ret['u'] = 1;
    ret['b'] = 2;
    ret['r'] = 3;
    ret['g'] = 4;
    return ret;
  }();
  return kCharIdxArray[c];
}

class Patterns {
 public:
  Patterns(std::vector<std::string_view> patterns) {
    for (std::string_view p : patterns) {
      Prefix* val = &root_;
      for (char c : p) {
        int idx = CharIdx(c);
        if (val->subs[idx] == nullptr) {
          own.push_back(std::make_unique<Prefix>());
          val->subs[idx] = own.back().get();
        }
        val = val->subs[idx];
      }
      val->match = true;
    }
  }

  bool CanMake(std::string_view test);
  int64_t CountMakable(std::string_view test);

 private:
  struct Prefix {
    bool match = false;
    std::array<Prefix*, 5> subs = {nullptr, nullptr, nullptr, nullptr, nullptr};
  };

  std::vector<std::unique_ptr<Prefix>> own;
  Prefix root_;
};

bool Patterns::CanMake(std::string_view test) {
  std::vector<int> hist(test.size() + 1, false);
  hist.back() = true;

  for (int i = test.size() - 1; i >= 0; --i) {
    const Prefix* val = &root_;
    for (int c_idx = i; val != nullptr; ++c_idx) {
      if (val->match) {
        hist[i] |= hist[c_idx];
      }
      if (c_idx == test.size()) break;
      int p_idx = CharIdx(test[c_idx]);
      val = val->subs[p_idx];
    }
  }

  return hist[0];
}

int64_t Patterns::CountMakable(std::string_view test) {
  std::vector<int64_t> hist(test.size() + 1, 0);
  hist.back() = 1;

  for (int i = test.size() - 1; i >= 0; --i) {
    const Prefix* val = &root_;
    for (int c_idx = i; val != nullptr; ++c_idx) {
      if (val->match) {
        hist[i] += hist[c_idx];
      }
      if (c_idx == test.size()) break;
      int p_idx = CharIdx(test[c_idx]);
      val = val->subs[p_idx];
    }
  }

  return hist[0];
}

}  // namespace

absl::StatusOr<std::string> Day_2024_19::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() < 3) return absl::InvalidArgumentError("Too short");
  std::vector<std::string_view> pattern_list = absl::StrSplit(input[0], ", ");
  for (std::string_view p : pattern_list) {
    for (char c : p) {
      if (CharIdx(c) < 0) return absl::InvalidArgumentError("Bad color");
    }
  }
  if (!input[1].empty()) return absl::InvalidArgumentError("No blank line");

  Patterns patterns(std::move(pattern_list));
  int count = 0;
  for (int i = 2; i < input.size(); ++i) {
    if (patterns.CanMake(input[i])) {
      ++count;
    }
  }
  return AdventReturn(count);
}

absl::StatusOr<std::string> Day_2024_19::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() < 3) return absl::InvalidArgumentError("Too short");
  std::vector<std::string_view> pattern_list = absl::StrSplit(input[0], ", ");
  for (std::string_view p : pattern_list) {
    for (char c : p) {
      if (CharIdx(c) < 0) return absl::InvalidArgumentError("Bad color");
    }
  }
  if (!input[1].empty()) return absl::InvalidArgumentError("No blank line");

  Patterns patterns(std::move(pattern_list));
  int64_t count = 0;
  for (int i = 2; i < input.size(); ++i) {
    count += patterns.CountMakable(input[i]);
  }
  return AdventReturn(count);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/19,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_19()); });

}  // namespace advent_of_code
