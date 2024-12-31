// http://adventofcode.com/2024/day/21

#include "advent_of_code/2024/day21/day21.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/fast_board.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/mod.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class ButtonPusher : public BFSInterface<ButtonPusher> {
 public:
  ButtonPusher(std::string_view target) : target_(target) {}

  const ButtonPusher& identifier() const override { return *this; }
  bool operator==(const ButtonPusher& o) const {
    return target_ == o.target_ && human_ == o.human_ && robot_1_ == o.robot_1_ && robot_2_ == o.robot_2_ && robot_3_ == o.robot_3_;
  }

  template <typename H>
  friend H AbslHashValue(H h, const ButtonPusher& bp) {
    return H::combine(std::move(h), bp.target_, bp.human_, bp.robot_1_, bp.robot_2_, bp.robot_3_);
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const ButtonPusher& bp) {
    absl::Format(&sink, "target:%s {%v, %v, %v, %v}", bp.target_, bp.human_, bp.robot_1_, bp.robot_2_, bp.robot_3_);
  }

  bool IsFinal() const override { return target_.empty(); }

  void AddNextSteps(State* state) const override {
    // Move human.
    for (Point d : Cardinal::kFourDirs) {
      if (OkArrow(human_ + d)) {
        ButtonPusher next = *this;
        next.human_ += d;
        state->AddNextStep(next);
      }
    }

    // Press human.
    {
      ButtonPusher next = *this;
      bool add_ok = true;
      if (std::optional<Point> p = ArrowDir(human_)) {
        next.robot_1_ += *p;
      } else if (std::optional<Point> p = ArrowDir(robot_1_)) {
        next.robot_2_ += *p;
      } else if (std::optional<Point> p = ArrowDir(robot_2_)) {
        next.robot_3_ += *p;
      } else if (std::optional<char> c = KeyPadChar(robot_3_)) {
        if (c == target_[0]) {
          next.target_ = next.target_.substr(1);
        } else {
          add_ok = false;
        }
      }
      if (add_ok) {
        state->AddNextStep(next);
      }
    }

  }

 private:
  static bool OkArrow(Point p) {
    static const PointRectangle kArrowBounds = {{0, 0}, {2, 1}};
    static const Point kBadArrowSpot = {0, 0};
    return kArrowBounds.Contains(p) && p != kBadArrowSpot;
  }

  static std::optional<Point> ArrowDir(Point p) {
    if (p == Point{0, 1}) return Cardinal::kNorth;
    if (p == Point{1, 0}) return Cardinal::kWest;
    if (p == Point{1, 1}) return Cardinal::kSouth;
    if (p == Point{1, 2}) return Cardinal::kEast;
    return std::nullopt;
  }

  static bool OkKeyPad(Point p) {
    static const PointRectangle kKeyPadBounds = {{0, 0}, {2, 3}};
    static const Point kBadKeyPadSpot = {0, 3};
    return kKeyPadBounds.Contains(p) && p != kBadKeyPadSpot;
  }

  static std::optional<char> KeyPadChar(Point p) {
    if (p == Point{0, 0}) return '7';
    if (p == Point{0, 1}) return '8';
    if (p == Point{0, 2}) return '9';
    if (p == Point{1, 0}) return '4';
    if (p == Point{1, 1}) return '5';
    if (p == Point{1, 2}) return '6';
    if (p == Point{2, 0}) return '1';
    if (p == Point{2, 1}) return '2';
    if (p == Point{2, 2}) return '3';
    if (p == Point{3, 1}) return '0';
    return std::nullopt;
  }

  std::string_view target_;
  Point human_ = {2, 0};
  Point robot_1_ = {2, 0};
  Point robot_2_ = {2, 0};
  Point robot_3_ = {2, 3};
};

const std::vector<std::string>& AllCombinations(int a, int b, char ac, char bc) {
  static absl::flat_hash_map<std::tuple<int, int, char, char>, std::vector<std::string>> memo;
  auto key = std::make_tuple(a, b, ac, bc);
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

using State = std::array<Point, 4>;

absl::flat_hash_map<State, int64_t> ToDirectional(std::string_view line, State in, int level) {
  if (level == 3) return {{in, line.size()}};

  static const absl::flat_hash_map<char, Point> kRevDirectional = {
    {'^', {1, 0}}, {'A', {2, 0}},
    {'<', {0, 1}}, {'v', {1, 1}}, {'>', {2, 1}}
  };

  absl::flat_hash_map<State, int64_t> states = {{in, 0}};

  std::string prefix(level * 2, ' ');

  for (char c : line) {
    absl::flat_hash_map<State, int64_t> new_states;
    for (const auto& [state, steps] : states) {
      Point pointer = state[level];

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
  
      for (std::string path : AllCombinations(dx, dy, dxc, dyc)) {
        path.append(1, 'A');
        if (path == bad_path) continue;
 
         {
          Point test = state[level];
          for (char c : path) {
            test += Cardinal::Parse(c);
            CHECK_NE(test, Point({0, 0})) << path << ": " << state[level] << "-" << dest << " ? " << bad_path;
          }
        }

        for (const auto& [sub_state, sub_steps] : ToDirectional(path, state, level + 1)) {
          State new_state = sub_state;
          new_state[level] = dest;
          auto [it, inserted] = new_states.emplace(new_state, steps + sub_steps);
          if (!inserted) {
            it->second = std::min(it->second, steps + sub_steps);
          }
        }
      }
    }
    states = std::move(new_states);
  }

  return states;
}

absl::flat_hash_map<State, int64_t> ToKeyPad(std::string_view line) {
  static const absl::flat_hash_map<char, Point> kRevKeyPad = {
    {'7', {0, 0}}, {'8', {1, 0}}, {'9', {2, 0}},
    {'4', {0, 1}}, {'5', {1, 1}}, {'6', {2, 1}},
    {'1', {0, 2}}, {'2', {1, 2}}, {'3', {2, 2}},
    {'0', {1, 3}}, {'A', {2, 3}}
  };

  absl::flat_hash_map<State, int64_t> states = {
    {{Point{2, 3}, {2, 0}, {2, 0}}, 0}
  };

  for (char c : line) {
    absl::flat_hash_map<State, int64_t> new_states;
    for (const auto& [state, steps] : states) {
      Point pointer = state[0];

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
  
      for (std::string path : AllCombinations(dx, dy, dxc, dyc)) {
        path.append(1, 'A');
        if (path == bad_path) continue;

        {
          Point test = state[0];
          for (char c : path) {
            test += Cardinal::Parse(c);
            CHECK_NE(test, Point({0, 3})) << path << ": " << state[0] << "-" << dest << " ? " << bad_path;
          }
        }

        for (const auto& [sub_state, sub_steps] : ToDirectional(path, state, 1)) {
          State new_state = sub_state;
          new_state[0] = dest;
          auto [it, inserted] = new_states.emplace(new_state, steps + sub_steps);
          if (!inserted) {
            it->second = std::min(it->second, steps + sub_steps);
          }
        }
      }
    }
    states = std::move(new_states);
  }

  return states;
}

}  // namespace

absl::StatusOr<std::string> Day_2024_21::Part1(
    absl::Span<std::string_view> input) const {
  int sum = 0;
  for (std::string_view line : input) {
    int num_code = 0;
    for (char c : line) {
      if (!std::isdigit(c)) break;
      num_code = num_code * 10 + (c - '0');
    }

    int64_t best_path_size = std::numeric_limits<int64_t>::max();
    for (const auto& [state, steps] : ToKeyPad(line)) {
      best_path_size = std::min(best_path_size, steps);
    }
    LOG(ERROR) << line << ": " << best_path_size << " * " << num_code;
    sum += best_path_size * num_code;
  }

  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2024_21::Part2(
    absl::Span<std::string_view> input) const {
  return absl::UnimplementedError("Problem not known");
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/21,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_21()); });

}  // namespace advent_of_code
