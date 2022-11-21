// http://adventofcode.com/2019/day/18

#include "advent_of_code/2019/day18/day18.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/opt_cmp.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

static void CharJoin(std::string* out, char c) { out->append(1, c); }

struct NKeyState {
  std::string robot_key;
  int have_key_bv;

  bool operator==(const NKeyState& other) const {
    return robot_key == other.robot_key && have_key_bv == other.have_key_bv;
  }

  template <typename H>
  friend H AbslHashValue(H h, const NKeyState& key_state) {
    return H::combine(std::move(h), key_state.robot_key, key_state.have_key_bv);
  }
};

class Board {
 public:
  Board(const CharBoard& board) : board_(board){};

  absl::Status InitializeBoard() {
    for (Point cur_point : board_.range()) {
      if (board_[cur_point] == '@') {
        robots_.push_back(cur_point);
      } else if (board_[cur_point] >= 'a' && board_[cur_point] <= 'z') {
        keys_[board_[cur_point]] = cur_point;
      }
    }
    return absl::OkStatus();
  }

  bool CanAdvance(Point p, const absl::flat_hash_set<char>& have_keys) {
    if (!board_.OnBoard(p)) return false;
    if (board_[p] == '#') return false;
    if (board_[p] >= 'A' && board_[p] <= 'Z') {
      return have_keys.contains(board_[p] - 'A' + 'a');
    }
    return true;
  }

  bool CanAdvanceAllKeys(Point p) {
    if (!board_.OnBoard(p)) return false;
    if (board_[p] == '#') return false;
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

    bool operator<(const KeyPath& o) const {
      if (required_keys.size() != o.required_keys.size()) {
        return required_keys.size() < o.required_keys.size();
      }
      return steps < o.steps;
    }

    std::string DebugString() const {
      char from_key_str[] = {from_key, '\0'};
      char to_key_str[] = {to_key, '\0'};
      return absl::StrCat(
          from_key_str, "->", to_key_str, "; ", from, "=>", to, "; ", steps,
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
    std::sort(paths.begin(), paths.end());
    return paths;
  }

  std::vector<KeyPath> FindAllKeysFrom(char from_key, Point from) {
    std::vector<KeyPath> ret;
    absl::flat_hash_set<Point> visited;
    visited.insert(from);
    std::deque<KeyPath> frontier;
    frontier.push_back(KeyPath{.from = from,
                               .to = from,
                               .steps = 0,
                               .from_key = from_key,
                               .to_key = '\0'});
    while (!frontier.empty()) {
      const KeyPath& cur = frontier.front();
      for (Point dir : Cardinal::kFourDirs) {
        Point next_to = cur.to + dir;
        if (visited.contains(next_to)) continue;
        visited.insert(next_to);
        if (CanAdvanceAllKeys(next_to)) {
          KeyPath next = cur;
          next.to = next_to;
          ++next.steps;
          char board_char = board_[next_to];
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

  absl::optional<int> MinStepsToAllKeys() {
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

  absl::optional<int> MinStepsRobotDP(
      const absl::flat_hash_map<char, std::vector<KeyPath>>& all_paths_idx) {
    absl::flat_hash_map<NKeyState, int> states =
        AllMinStatesForRobotsNKeys(all_paths_idx, keys_.size());
    absl::optional<int> min;
    for (const auto& pair : states) {
      min = opt_min(min, pair.second);
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
  const CharBoard& board_;
  std::vector<Point> robots_;
  absl::flat_hash_map<char, Point> keys_;
};

}  // namespace

absl::StatusOr<std::string> Day_2019_18::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> char_board = ParseAsBoard(input);
  if (!char_board.ok()) return char_board.status();
  Board b(*char_board);
  if (absl::Status st = b.InitializeBoard(); !st.ok()) return st;
  return IntReturn(b.MinStepsToAllKeys());
}

absl::StatusOr<std::string> Day_2019_18::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> char_board = ParseAsBoard(input);
  if (!char_board.ok()) return char_board.status();

  bool found = false;
  Point robot;
  for (Point p : char_board->range()) {
    if (!char_board->OnBoard(p + Cardinal::kNorthWest)) continue;
    if (!char_board->OnBoard(p + Cardinal::kSouthEast)) continue;
    if ((*char_board)[p] == '@') {
      if (found) return Error("Multiple starting robots");
      found = true;
      robot = p;
    }
  }
  if (!found) return absl::InvalidArgumentError("Could not edit board");

  (*char_board)[robot] = '#';
  (*char_board)[robot + Cardinal::kNorth] = '#';
  (*char_board)[robot + Cardinal::kSouth] = '#';
  (*char_board)[robot + Cardinal::kWest] = '#';
  (*char_board)[robot + Cardinal::kEast] = '#';
  (*char_board)[robot + Cardinal::kNorthEast] = '@';
  (*char_board)[robot + Cardinal::kNorthWest] = '@';
  (*char_board)[robot + Cardinal::kSouthEast] = '@';
  (*char_board)[robot + Cardinal::kSouthWest] = '@';

  VLOG(1) << "\n" << *char_board;

  Board b(*char_board);
  if (absl::Status st = b.InitializeBoard(); !st.ok()) return st;

  return IntReturn(b.MinStepsToAllKeys());
}

}  // namespace advent_of_code
