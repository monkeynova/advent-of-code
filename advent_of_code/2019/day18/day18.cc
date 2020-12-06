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

class Board {
 public:
  Board(const std::vector<absl::string_view>& board) : board_(board) {};

  absl::Status InitializeBoard() {
    for (int y = 0; y < board_.size(); ++y) {
      for (int x = 0; x < board_[y].size(); ++x) {
        Point cur_point = Point{.x = x, .y = y};
        if (board_[y][x] == '@') {
          pos_ = cur_point;
        } else if (board_[y][x] >= 'a' && board_[y][x] <= 'z') {
          keys_[board_[y][x]] = cur_point;
        } else if (board_[y][x] >= 'A' && board_[y][x] <= 'Z') {
          doors_[board_[y][x] + 'a' - 'A'] = cur_point;
        }
      }
    }
    return absl::OkStatus();
  }

  struct ReachableKey {
    int steps;
    char key;
    Point pos;
  };

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
      return absl::StrCat(from_key_str, "->", to_key_str, "; ", from.DebugString(), "=>",
      to.DebugString(), "; ", steps, " steps; requires=", absl::StrJoin(required_keys, "", &CharJoin));
    }
  };

  std::vector<KeyPath> AllKeyPaths() {
    std::vector<KeyPath> paths;
    std::vector<KeyPath> to_add = FindAllKeysFrom('@', pos_);
    paths.insert(paths.end(), to_add.begin(), to_add.end());
    for (const auto& key_and_pos : keys_) {
      to_add = FindAllKeysFrom(key_and_pos.first, key_and_pos.second);
      paths.insert(paths.end(), to_add.begin(), to_add.end());
    }
    std::sort(paths.begin(), paths.end(),
    [](const KeyPath& a, const KeyPath& b) {
      if (a.required_keys.size() < b.required_keys.size()) return true;
      if (b.required_keys.size() < a.required_keys.size()) return false;
      return a.steps < b.steps;
    });
    return paths;
  }

  std::vector<KeyPath> FindAllKeysFrom(char from_key, Point from) {
    std::vector<KeyPath> ret;
    absl::flat_hash_set<Point> visited;
    visited.insert(from);
    std::deque<KeyPath> frontier;
    frontier.push_back({.from = from, .to = from, .steps = 0, .from_key = from_key, .to_key = '\0'});
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

  std::vector<ReachableKey> FindReachableKeys(const absl::flat_hash_set<char>& have_keys) {
    struct Path {
      Point pos;
      int steps;
    };
    std::vector<ReachableKey> ret;
    absl::flat_hash_set<Point> visited;
    std::deque<Path> frontier;
    frontier.push_back({.pos = pos_, .steps = 0});
    while (!frontier.empty()) {
      Path cur = frontier.front();
      frontier.pop_front();
      for (Point dir : Cardinal::kAll) {
        Point next = cur.pos + dir;
        if (visited.contains(next)) continue;
        visited.insert(next);
        if (CanAdvance(next, have_keys)) {
          if (board_[next.y][next.x] >= 'a' && board_[next.y][next.x] <= 'z' &&
              !have_keys.contains(board_[next.y][next.x])) {
            ret.push_back({.steps = cur.steps + 1, .pos = next, .key = board_[next.y][next.x]});
            if (have_keys.size() + ret.size() == keys_.size()) {
              frontier.clear();
              break;
            }
          } else {
            frontier.push_back({.pos = next, .steps = cur.steps + 1});
          }
        }
      }
    }
    std::sort(ret.begin(), ret.end(),
              [](const ReachableKey& a, const ReachableKey& b) {
                return a.steps < b.steps;
              });
    return ret;
  };

  absl::StatusOr<absl::optional<int>> MinStepsToAllKeys() {
    std::vector<KeyPath> all_paths = AllKeyPaths();
    absl::flat_hash_map<Point, std::vector<KeyPath>> all_paths_idx;
    absl::flat_hash_map<Point, std::vector<KeyPath>> all_paths_rev_idx;
    for (KeyPath& path : all_paths) {
      VLOG(1) << path.DebugString();
      path.required_keys_bv = 0;
      for (char k : path.required_keys) {
        path.required_keys_bv |= (1 << (k - 'a'));
      }
      all_paths_idx[path.from].push_back(path);
      all_paths_rev_idx[path.to].push_back(path);
    }
    return MinStepsDynamicProgramming(all_paths_rev_idx);
    int have_keys_bv = 0;
    return MinStepsToAllKeysWithPaths(0, have_keys_bv, all_paths_idx, 0, absl::nullopt);
    absl::flat_hash_set<char> have_keys;
    return MinStepsToAllKeysImpl(&have_keys, 0, absl::nullopt);
  }

  absl::StatusOr<absl::optional<int>> MinStepsDynamicProgramming(
    const absl::flat_hash_map<Point, std::vector<KeyPath>>& all_paths_rev_idx) {
    absl::flat_hash_map<std::pair<int, Point>, absl::optional<int>> keys_bv_and_pos_to_min_path;
    absl::optional<int> min;
    for (const auto& key_and_pos : keys_) {
      absl::optional<int> next_min =
          MinStepsDynamicProgrammingState(&keys_bv_and_pos_to_min_path,
                                          ((1 << keys_.size()) - 1) /*all keys*/,
                                          key_and_pos.second, all_paths_rev_idx);
      if (next_min) {
        if (!min || *min > *next_min) {
          min = next_min;
        }
      }
    }
    return min;
  }

  absl::optional<int> MinStepsDynamicProgrammingState(
    absl::flat_hash_map<std::pair<int, Point>, absl::optional<int>>* keys_bv_and_pos_to_min_path,
    int have_keys_bv, Point pos,
    const absl::flat_hash_map<Point, std::vector<KeyPath>>& all_paths_rev_idx) {
    VLOG(1) << "MinStepsDynamicProgrammingState keys=0x" << std::hex << have_keys_bv << " @" << pos;
    std::pair<int, Point> cache_key(have_keys_bv, pos);
    if (auto it = keys_bv_and_pos_to_min_path->find(cache_key);
        it != keys_bv_and_pos_to_min_path->end()) {
      return it->second;
    }

    absl::optional<int> min;
    if (auto it = all_paths_rev_idx.find(pos); it != all_paths_rev_idx.end()) {
      const std::vector<KeyPath>& paths = it->second;
      if (!paths.empty()) {
        have_keys_bv &= ~(1 << (paths[0].to_key - 'a'));
      }
      for (const KeyPath& path : paths) {
        VLOG(2) << "  path=" << path.DebugString();
        if (have_keys_bv == 0) {
          if (path.from_key == '@') return path.steps;
        } else if (path.from_key != '@' && 
                   (have_keys_bv & (1 << (path.from_key - 'a'))) &&
                   (have_keys_bv & path.required_keys_bv) == path.required_keys_bv) {
          absl::optional<int> next_min = MinStepsDynamicProgrammingState(
            keys_bv_and_pos_to_min_path, have_keys_bv, path.from,
            all_paths_rev_idx);
          if (next_min) {
            if (!min || *min > *next_min + path.steps) {
              min = *next_min + path.steps;
            }
          }
        }
      }
    }

    keys_bv_and_pos_to_min_path->emplace(cache_key, min);
    if (keys_bv_and_pos_to_min_path->size() % 777777 == 0){
      LOG(WARNING) << "MinStepsDynamicProgrammingState progress: " << keys_bv_and_pos_to_min_path->size()
       << "/" << (1 << keys_.size()) * keys_.size();
    }
    return min;
  }


  absl::StatusOr<absl::optional<int>> MinStepsToAllKeysWithPaths(
      int key_count,
      int have_keys_bv,
      const absl::flat_hash_map<Point, std::vector<KeyPath>>& all_paths_idx,
      int base_steps, absl::optional<int> best_so_far) {
    VLOG_IF(1, key_count < 5) << "MinStepsToAllKeysWithPaths: " << key_count << " of " << keys_.size()
            << "; steps=" << base_steps << "; from=" << pos_ << "; best=" 
            << (best_so_far ? *best_so_far : -1);
    if (key_count == keys_.size()) return base_steps;

    Point save_pos = pos_;
    auto it = all_paths_idx.find(pos_);
    if (it == all_paths_idx.end()) return absl::nullopt;
  
    for (const auto& key_path : it->second) {
      if (best_so_far && base_steps + key_path.steps > *best_so_far) continue;
      int key_bit = (1 << (key_path.to_key - 'a'));
      if (have_keys_bv & key_bit) continue;
      if ((have_keys_bv & key_path.required_keys_bv) != key_path.required_keys_bv) continue;
      have_keys_bv |= key_bit;
      pos_ = key_path.to;

      absl::StatusOr<absl::optional<int>> next_steps =
          MinStepsToAllKeysWithPaths(key_count + 1, have_keys_bv, all_paths_idx, base_steps + key_path.steps, best_so_far);
      if (!next_steps.ok()) return next_steps.status();
      if (*next_steps) {
        VLOG(3) << "  This Steps: " << **next_steps;
        if (!best_so_far || *best_so_far > **next_steps) {
          best_so_far = **next_steps;
        }
      }

      have_keys_bv &= ~key_bit;
      pos_ = save_pos;
    }
    return best_so_far;
  }

  absl::StatusOr<absl::optional<int>> MinStepsToAllKeysImpl(
      absl::flat_hash_set<char>* have_keys, int base_steps, absl::optional<int> best_so_far) {
    VLOG_EVERY_N(1, 7777) << "MinStepsToAllKeys: " << have_keys->size() << " of " << keys_.size() << "; best=" 
            << (best_so_far ? *best_so_far : -1);
    if (have_keys->size() == keys_.size()) return base_steps;
  
    std::vector<ReachableKey> reachable_keys = FindReachableKeys(*have_keys);
    Point save_pos = pos_;
    for (const ReachableKey& key : reachable_keys) {
      if (best_so_far && base_steps + key.steps > *best_so_far) continue;
      VLOG(2) << "Trying reachable: " << key.key;
      have_keys->insert(key.key);
      pos_ = key.pos;
      absl::StatusOr<absl::optional<int>> next_steps =
          MinStepsToAllKeysImpl(have_keys, base_steps + key.steps, best_so_far);
      if (!next_steps.ok()) return next_steps.status();
      if (*next_steps) {
        VLOG(3) << "  This Steps: " << **next_steps;
        if (!best_so_far || *best_so_far > **next_steps) {
          best_so_far = **next_steps;
        }
      }
      have_keys->erase(key.key);
      pos_ = save_pos;
    }
    return best_so_far;
  }

 private:
  const std::vector<absl::string_view>& board_;
  Point pos_;
  absl::flat_hash_map<char, Point> keys_;
  absl::flat_hash_map<char, Point> doors_;
};

absl::StatusOr<std::vector<std::string>> Day18_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  Board b(input);
  if (absl::Status st = b.InitializeBoard(); !st.ok()) return st;
  absl::StatusOr<absl::optional<int>> steps = b.MinStepsToAllKeys();
  if (!steps.ok()) return steps.status();
  if (!*steps) return absl::InvalidArgumentError("No Path found");
  return std::vector<std::string>{absl::StrCat(**steps)};
}

absl::StatusOr<std::vector<std::string>> Day18_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  return std::vector<std::string>{""};
}
