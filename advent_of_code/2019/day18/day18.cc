#include "advent_of_code/2019/day18/day18.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

static void CharJoin(std::string* out, char c) { out->append(1, c); }

struct NKeyState {
  std::string robot_key;
  int have_key_bv;
  bool operator==(const NKeyState& other) const {
    return robot_key == other.robot_key && have_key_bv == other.have_key_bv;
  }
};
template <typename H>
H AbslHashValue(H h, const NKeyState& key_state) {
  return H::combine(std::move(h), key_state.robot_key, key_state.have_key_bv);
}

class Board {
 public:
  Board(const std::vector<absl::string_view>& board) : board_(board){};

  absl::Status InitializeBoard() {
    for (int y = 0; y < board_.size(); ++y) {
      for (int x = 0; x < board_[y].size(); ++x) {
        Point cur_point = Point{.x = x, .y = y};
        if (board_[y][x] == '@') {
          robots_.push_back(cur_point);
        } else if (board_[y][x] >= 'a' && board_[y][x] <= 'z') {
          keys_[board_[y][x]] = cur_point;
        }
      }
    }
    return absl::OkStatus();
  }

  bool CanAdvance(Point p, const absl::flat_hash_set<char>& have_keys) {
    if (p.y < 0) return false;
    if (p.y >= board_.size()) return false;
    if (p.x < 0) return false;
    if (p.x >= board_[p.y].size()) return false;
    if (board_[p.y][p.x] == '#') return false;
    if (board_[p.y][p.x] >= 'A' && board_[p.y][p.x] <= 'Z') {
      return have_keys.contains(board_[p.y][p.x] - 'A' + 'a');
    }
    return true;
  }

  bool CanAdvanceAllKeys(Point p) {
    if (p.y < 0) return false;
    if (p.y >= board_.size()) return false;
    if (p.x < 0) return false;
    if (p.x >= board_[p.y].size()) return false;
    if (board_[p.y][p.x] == '#') return false;
    return true;
  }

  struct KeyPath {
    Point from;
    Point to;
    int steps;
    absl::flat_hash_set<char> required_keys;
    int required_keys_bv = 0;
    char from_key;
    char to_key;
    std::string DebugString() const {
      char from_key_str[] = {from_key, '\0'};
      char to_key_str[] = {to_key, '\0'};
      return absl::StrCat(
          from_key_str, "->", to_key_str, "; ", from.DebugString(), "=>",
          to.DebugString(), "; ", steps,
          " steps; requires=", absl::StrJoin(required_keys, "", &CharJoin));
    }
  };

  std::vector<KeyPath> AllKeyPaths() {
    std::vector<KeyPath> paths;
    std::vector<KeyPath> to_add;
    for (Point pos : robots_) {
      to_add = FindAllKeysFrom('@', pos);
      paths.insert(paths.end(), to_add.begin(), to_add.end());
    }
    for (const auto& key_and_pos : keys_) {
      to_add = FindAllKeysFrom(key_and_pos.first, key_and_pos.second);
      paths.insert(paths.end(), to_add.begin(), to_add.end());
    }
    std::sort(paths.begin(), paths.end(),
              [](const KeyPath& a, const KeyPath& b) {
                if (a.required_keys.size() < b.required_keys.size())
                  return true;
                if (b.required_keys.size() < a.required_keys.size())
                  return false;
                return a.steps < b.steps;
              });
    return paths;
  }

  std::vector<KeyPath> FindAllKeysFrom(char from_key, Point from) {
    std::vector<KeyPath> ret;
    absl::flat_hash_set<Point> visited;
    visited.insert(from);
    std::deque<KeyPath> frontier;
    frontier.push_back({.from = from,
                        .to = from,
                        .steps = 0,
                        .from_key = from_key,
                        .to_key = '\0'});
    while (!frontier.empty()) {
      const KeyPath& cur = frontier.front();
      for (Point dir : Cardinal::kAll) {
        Point next_to = cur.to + dir;
        if (visited.contains(next_to)) continue;
        visited.insert(next_to);
        if (CanAdvanceAllKeys(next_to)) {
          KeyPath next = cur;
          next.to = next_to;
          ++next.steps;
          char board_char = board_[next_to.y][next_to.x];
          if (board_char >= 'a' && board_char <= 'z') {
            next.to_key = board_char;
            ret.push_back(next);
          }
          if (board_char >= 'A' && board_char <= 'Z') {
            next.required_keys.insert(board_char - 'A' + 'a');
          }
          frontier.push_back(next);
        }
      }
      frontier.pop_front();
    }
    return ret;
  };

  absl::StatusOr<absl::optional<int>> MinStepsToAllKeys() {
    std::vector<KeyPath> all_paths = AllKeyPaths();
    absl::flat_hash_map<char, std::vector<KeyPath>> all_paths_idx_by_key;
    for (KeyPath& path : all_paths) {
      VLOG(1) << path.DebugString();
      path.required_keys_bv = 0;
      for (char k : path.required_keys) {
        path.required_keys_bv |= (1 << (k - 'a'));
      }
      all_paths_idx_by_key[path.from_key].push_back(path);
    }
    return MinStepsRobotDP(all_paths_idx_by_key);
  }

  absl::StatusOr<absl::optional<int>> MinStepsRobotDP(
      const absl::flat_hash_map<char, std::vector<KeyPath>>& all_paths_idx) {
    absl::flat_hash_map<NKeyState, int> states =
        AllMinStatesForRobotsNKeys(all_paths_idx, keys_.size());
    absl::optional<int> min;
    for (const auto& pair : states) {
      if (!min || *min > pair.second) {
        min = pair.second;
      }
    }
    return min;
  }

  absl::flat_hash_map<NKeyState, int> AllMinStatesForRobotsNKeys(
      const absl::flat_hash_map<char, std::vector<KeyPath>>& all_paths_idx,
      int num_keys) {
    VLOG(1) << "AllMinStatesForRobotsNKeys (Start) " << num_keys;
    absl::flat_hash_map<NKeyState, int> out_states;
    if (num_keys == 0) {
      std::string robot_key(robots_.size(), '@');
      NKeyState base_state{
          .robot_key = robot_key,
          .have_key_bv = 0,
      };
      out_states[base_state] = 0;
      return out_states;
    }
    absl::flat_hash_map<NKeyState, int> in_states =
        AllMinStatesForRobotsNKeys(all_paths_idx, num_keys - 1);
    for (const auto& pair : in_states) {
      const NKeyState& in_state = pair.first;
      int in_steps = pair.second;
      for (int i = 0; i < in_state.robot_key.size(); ++i) {
        if (auto it = all_paths_idx.find(in_state.robot_key[i]);
            it != all_paths_idx.end()) {
          for (const KeyPath& path : it->second) {
            int to_bv = (1 << (path.to_key - 'a'));
            if (in_state.have_key_bv & to_bv) continue;
            if ((in_state.have_key_bv & path.required_keys_bv) !=
                path.required_keys_bv)
              continue;
            if (in_state.robot_key[i] == '@' && path.from != robots_[i])
              continue;
            NKeyState out_state = in_state;
            out_state.robot_key[i] = path.to_key;
            out_state.have_key_bv |= to_bv;
            if (auto it = out_states.find(out_state); it != out_states.end()) {
              out_states[out_state] =
                  std::min(out_states[out_state], in_steps + path.steps);
            } else {
              out_states[out_state] = in_steps + path.steps;
            }
          }
        }
      }
    }
    VLOG(1) << "AllMinStatesForRobotsNKeys (Done) " << num_keys << "; "
            << out_states.size();

    return out_states;
  }

 private:
  const std::vector<absl::string_view>& board_;
  std::vector<Point> robots_;
  absl::flat_hash_map<char, Point> keys_;
};

absl::StatusOr<std::vector<std::string>> Day18_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  Board b(input);
  if (absl::Status st = b.InitializeBoard(); !st.ok()) return st;
  absl::StatusOr<absl::optional<int>> steps = b.MinStepsToAllKeys();
  if (!steps.ok()) return steps.status();
  if (!*steps) return absl::InvalidArgumentError("No Path found");
  return IntReturn(**steps);
}

absl::StatusOr<std::vector<std::string>> Day18_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  std::vector<std::string> altered_input;
  for (absl::string_view str : input) {
    altered_input.push_back(std::string(str));
  }
  bool found = false;
  for (int y = 1; !found && y < altered_input.size() - 1; ++y) {
    for (int x = 1; !found && x < altered_input[y].size() - 1; ++x) {
      if (altered_input[y][x] == '@') {
        found = true;
        altered_input[y - 1][x - 1] = '@';
        altered_input[y + 1][x - 1] = '@';
        altered_input[y - 1][x + 1] = '@';
        altered_input[y + 1][x + 1] = '@';
        altered_input[y - 1][x] = '#';
        altered_input[y + 1][x] = '#';
        altered_input[y][x - 1] = '#';
        altered_input[y][x + 1] = '#';
        altered_input[y][x] = '#';
      }
    }
  }
  if (!found) return absl::InvalidArgumentError("Could not edit board");

  std::vector<absl::string_view> altered_input_view;
  for (const std::string& str : altered_input) {
    altered_input_view.push_back(str);
  }

  VLOG(1) << "\n" << absl::StrJoin(altered_input_view, "\n");

  Board b(altered_input_view);
  if (absl::Status st = b.InitializeBoard(); !st.ok()) return st;

  absl::StatusOr<absl::optional<int>> steps = b.MinStepsToAllKeys();
  if (!steps.ok()) return steps.status();
  if (!*steps) return absl::InvalidArgumentError("No Path found");
  return IntReturn(**steps);
}
