// http://adventofcode.com/2024/day/21

#include "advent_of_code/2024/day21/day21.h"

#include "absl/container/flat_hash_map.h"
#include "absl/log/log.h"
#include "absl/strings/str_join.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

namespace {

const std::vector<std::string>& AllCombinations(int a, int b, char ac,
                                                char bc) {
  auto key = std::make_tuple(a, b, ac, bc);
  static absl::flat_hash_map<decltype(key), std::vector<std::string>> memo;
  if (auto it = memo.find(key); it != memo.end()) {
    return it->second;
  }

  std::vector<std::string> ret;
  if (a == 0 && b == 0) ret.push_back("");
  if (a > 0) {
    std::vector<std::string> sub = AllCombinations(a - 1, b, ac, bc);
    for (std::string& s : sub) {
      s.append(1, ac);
      ret.push_back(std::move(s));
    }
  }
  if (b > 0) {
    std::vector<std::string> sub = AllCombinations(a, b - 1, ac, bc);
    for (std::string& s : sub) {
      s.append(1, bc);
      ret.push_back(std::move(s));
    }
  }

  VLOG(2) << "AllCombinations(" << a << "," << b << ","
          << std::string_view(&ac, 1) << "," << std::string_view(&bc, 1)
          << ") = " << absl::StrJoin(ret, ",");

  return memo[key] = ret;
}

int64_t ToDirectional(std::string_view line, int max_level, int level) {
  if (level == max_level) return line.size();

  auto key = std::make_tuple(std::string(line), max_level, level);
  static absl::flat_hash_map<decltype(key), int64_t> memo;
  if (auto it = memo.find(key); it != memo.end()) {
    return it->second;
  }

  static const absl::flat_hash_map<char, Point> kRevDirectional = {
      {'^', {1, 0}},
      {'A', {2, 0}},
      {'<', {0, 1}},
      {'v', {1, 1}},
      {'>', {2, 1}}};

  Point pointer = {2, 0};

  int64_t best = 0;
  for (char c : line) {
    auto it = kRevDirectional.find(c);
    CHECK(it != kRevDirectional.end());
    Point dest = it->second;
    int dx = abs(dest.x - pointer.x);
    char dxc = dest.x > pointer.x ? '>' : '<';
    int dy = abs(dest.y - pointer.y);
    char dyc = dest.y > pointer.y ? 'v' : '^';

    std::string bad_path = "";
    if (dest.x == 0 && pointer.x != 0 && pointer.y == 0) {
      bad_path.append(dx, '<');
      bad_path.append(dy, 'v');
      bad_path.append(1, 'A');
    } else if (pointer.x == 0 && dest.x != 0 && dest.y == 0) {
      bad_path.append(dy, '^');
      bad_path.append(dx, '>');
      bad_path.append(1, 'A');
    }

    int64_t sub_best = std::numeric_limits<int64_t>::max();
    for (std::string path : AllCombinations(dx, dy, dxc, dyc)) {
      path.append(1, 'A');
      if (path == bad_path) continue;

      int64_t sub_steps = ToDirectional(path, max_level, level + 1);
      sub_best = std::min(sub_best, sub_steps);
    }
    best += sub_best;
    pointer = dest;
  }

  return memo[key] = best;
}

int64_t ToKeyPad(std::string_view line, int mids) {
  static const absl::flat_hash_map<char, Point> kRevKeyPad = {
      {'7', {0, 0}}, {'8', {1, 0}}, {'9', {2, 0}}, {'4', {0, 1}},
      {'5', {1, 1}}, {'6', {2, 1}}, {'1', {0, 2}}, {'2', {1, 2}},
      {'3', {2, 2}}, {'0', {1, 3}}, {'A', {2, 3}}};

  Point pointer = {2, 3};

  int64_t best = 0;
  for (char c : line) {
    auto it = kRevKeyPad.find(c);
    CHECK(it != kRevKeyPad.end());
    Point dest = it->second;
    int dx = abs(dest.x - pointer.x);
    char dxc = dest.x > pointer.x ? '>' : '<';
    int dy = abs(dest.y - pointer.y);
    char dyc = dest.y > pointer.y ? 'v' : '^';

    std::string bad_path = "";
    if (dest.x == 0 && pointer.x != 0 && pointer.y == 3) {
      bad_path.append(dx, '<');
      bad_path.append(dy, '^');
      bad_path.append(1, 'A');
    } else if (pointer.x == 0 && dest.x != 0 && dest.y == 3) {
      bad_path.append(dy, 'v');
      bad_path.append(dx, '>');
      bad_path.append(1, 'A');
    }

    int64_t sub_best = std::numeric_limits<int64_t>::max();
    for (std::string path : AllCombinations(dx, dy, dxc, dyc)) {
      path.append(1, 'A');
      if (path == bad_path) continue;

      int64_t sub_steps = ToDirectional(path, mids + 1, 1);
      sub_best = std::min(sub_best, sub_steps);
    }
    best += sub_best;
    pointer = dest;
  }

  return best;
}

}  // namespace

absl::StatusOr<std::string> Day_2024_21::Part1(
    absl::Span<std::string_view> input) const {
  int64_t sum = 0;
  for (std::string_view line : input) {
    int64_t num_code = 0;
    for (char c : line) {
      if (!std::isdigit(c)) break;
      num_code = num_code * 10 + (c - '0');
    }

    int64_t best_path_size = ToKeyPad(line, 2);
    VLOG(2) << line << ": " << best_path_size << " * " << num_code;
    sum += best_path_size * num_code;
  }

  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2024_21::Part2(
    absl::Span<std::string_view> input) const {
  int64_t sum = 0;
  for (std::string_view line : input) {
    int64_t num_code = 0;
    for (char c : line) {
      if (!std::isdigit(c)) break;
      num_code = num_code * 10 + (c - '0');
    }

    int64_t best_path_size = ToKeyPad(line, 25);
    VLOG(2) << line << ": " << best_path_size << " * " << num_code;
    sum += best_path_size * num_code;
  }

  return AdventReturn(sum);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/21,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_21()); });

}  // namespace advent_of_code
