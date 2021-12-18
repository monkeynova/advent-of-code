#include "advent_of_code/2021/day18/day18.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct SnailFish {
  struct Rec {
    int depth;
    int value;
  };
  std::vector<Rec> values;
  
  friend std::ostream& operator<<(std::ostream& o, const SnailFish& s) {
    int depth = 0;
    std::vector<bool> print_left;
    for (const auto& r : s.values) {
      if (!print_left.empty() && print_left.back()) {
        o << ",";
      }
      while (depth < r.depth) {
        ++depth;
        o << "[";
        print_left.push_back(false);
      }
      o << r.value;
      while (!print_left.empty() && print_left.back()) {
        o << "]";
        print_left.pop_back();
        --depth;
      }
      if (!print_left.empty()) {
        print_left.back() = true;
      }
    }
    while (!print_left.empty() && print_left.back()) {
      o << "]";
      print_left.pop_back();
    }
    return o;
  }

  static absl::StatusOr<SnailFish> Parse(absl::string_view& line) {
    int depth = 0;
    SnailFish ret;
    for (int i = 0; i < line.size(); ++i) {
      if (line[i] == '[') { ++depth; continue; }
      else if (line[i] == ',') { continue; }
      else if (line[i] == ']') { --depth; continue; }
      else if (line[i] >= '0' && line[i] <= '9') {
        int value = 0;
        while (line[i] >= '0' && line[i] <= '9') {
          value = value * 10 + line[i] - '0';
          ++i;
        }
        --i;
        ret.values.push_back({.depth = depth, .value = value});
      } else {
        return Error("Bad line");
      }
    }
    return ret;
  }
  SnailFish Add(const SnailFish& other) const {
    SnailFish ret;
    for (const auto &v : values) {
      ret.values.push_back({.depth = v.depth + 1, .value = v.value});
    }
    for (const auto& v : other.values) {
      ret.values.push_back({.depth = v.depth + 1, .value = v.value});
    }
    VLOG(2) << "Add";
    VLOG(2) << ret;
    ret.Reduce();
    return ret;
  }
  int64_t Magnitude() const {
    int depth = 0;
    std::vector<bool> on_left;
    std::vector<int> stack;
    VLOG(3) << "Magnitude: " << *this;
    for (const auto& r : values) {
      while (depth < r.depth) {
        ++depth;
        on_left.push_back(false);
      }
      int value = r.value;
      if (!on_left.back()) {
        VLOG(3) << "push: " << value;
        stack.push_back(value);
        on_left.back() = true;
      } else {
        while (!on_left.empty() && on_left.back()) {
          int new_val = stack.back() * 3 + 2 * value;
          VLOG(3) << "3 * " << stack.back() << " + 2 * " << value << " = " << new_val;
          stack.pop_back();
          value = new_val;
          --depth;
          on_left.pop_back();
        }
        stack.push_back(value);
        if (!on_left.empty()) {
          on_left.back() = true;
        }
      }
    }
    CHECK_EQ(stack.size(), 1);
    return stack[0];
  }
  void Reduce() {
    bool work_done = true;
    while (work_done) {
      VLOG(2) << "Reduce";
      VLOG(2) << *this;
      work_done = false;
      work_done = TryExplode();
      if (work_done) continue;
      work_done = TrySplit();
    }
  }
  bool TryExplode() {
    for (int i = 0; i < values.size() - 1; ++i) {
      if (values[i].depth > 4) {
        if (i > 0) values[i - 1].value += values[i].value;
        CHECK(values[i + 1].depth > 4);
        if (i + 2 < values.size()) {
          values[i + 2].value += values[i + 1].value;
        }
        values[i].value = 0;
        --values[i].depth;
        for (int j = i + 1; j < values.size() - 1; ++j) {
          values[j] = values[j + 1];
        }
        values.resize(values.size() - 1);
        return true;
      }
    }
    return false;
  }
  bool TrySplit() {
    for (int i = 0; i < values.size(); i++) {
      if (values[i].value >= 10) {
        values.resize(values.size() + 1);
        for (int j = values.size() - 1; j > i + 1; --j) {
          values[j] = values[j - 1];
        }
        values[i + 1].value = (values[i].value + 1) / 2;
        values[i + 1].depth = values[i].depth + 1;
        values[i].value = values[i].value / 2;
        values[i].depth = values[i].depth + 1;
        return true;
      }
    }
    return false;
  }
};

}  // namespace

absl::StatusOr<std::string> Day_2021_18::Part1(
    absl::Span<absl::string_view> input) const {
  SnailFish total;
  bool first = true;
  for (absl::string_view in_str : input) {
    absl::StatusOr<SnailFish> in = SnailFish::Parse(in_str);    
    if (!in.ok()) return in.status();
    VLOG(2) << in_str;
    VLOG(2) << *in;
    if (first) {
      total = std::move(*in);
      first = false;
    } else {
      total = total.Add(std::move(*in));
    }
    VLOG(2) << "Total: " << total;
  }
  return IntReturn(total.Magnitude());
}

absl::StatusOr<std::string> Day_2021_18::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<SnailFish> values;
  for (absl::string_view in_str : input) {
    absl::StatusOr<SnailFish> in = SnailFish::Parse(in_str);    
    if (!in.ok()) return in.status();
    values.push_back(*in);
  }
  int max = 0;
  for (const auto& sf1 : values) {
    for (const auto& sf2 : values) {
      int test = sf1.Add(sf2).Magnitude();
      max = std::max(max, test);
    }
  }
  return IntReturn(max);
}

}  // namespace advent_of_code
