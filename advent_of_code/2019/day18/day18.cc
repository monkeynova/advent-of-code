// https://adventofcode.com/2019/day/18
//
// --- Day 18: Many-Worlds Interpretation ---
// ------------------------------------------
// 
// As you approach Neptune, a planetary security system detects you and
// activates a giant tractor beam on Triton! You have no choice but to
// land.
// 
// A scan of the local area reveals only one interesting feature: a
// massive underground vault. You generate a map of the tunnels (your
// puzzle input). The tunnels are too narrow to move diagonally.
// 
// Only one entrance (marked @) is present among the open passages
// (marked .) and stone walls (#), but you also detect an assortment of
// keys (shown as lowercase letters) and doors (shown as uppercase
// letters). Keys of a given letter open the door of the same letter: a
// opens A, b opens B, and so on. You aren't sure which key you need to
// disable the tractor beam, so you'll need to collect all of them.
// 
// For example, suppose you have the following map:
// 
// #########
// #b.A.@.a#
// #########
// 
// Starting from the entrance (@), you can only access a large door (A)
// and a key (a). Moving toward the door doesn't help you, but you can
// move 2 steps to collect the key, unlocking A in the process:
// 
// #########
// #b.....@#
// #########
// 
// Then, you can move 6 steps to collect the only other key, b:
// 
// #########
// #@......#
// #########
// 
// So, collecting every key took a total of 8 steps.
// 
// Here is a larger example:
// 
// ########################
// #f.D.E.e.C.b.A.@.a.B.c.#
// ######################.#
// #d.....................#
// ########################
// 
// The only reasonable move is to take key a and unlock door A:
// 
// ########################
// #f.D.E.e.C.b.....@.B.c.#
// ######################.#
// #d.....................#
// ########################
// 
// Then, do the same with key b:
// 
// ########################
// #f.D.E.e.C.@.........c.#
// ######################.#
// #d.....................#
// ########################
// 
// ...and the same with key c:
// 
// ########################
// #f.D.E.e.............@.#
// ######################.#
// #d.....................#
// ########################
// 
// Now, you have a choice between keys d and e. While key e is closer,
// collecting it now would be slower in the long run than collecting key
// d first, so that's the best choice:
// 
// ########################
// #f...E.e...............#
// ######################.#
// #@.....................#
// ########################
// 
// Finally, collect key e to unlock door E, then collect key f, taking a
// grand total of 86 steps.
// 
// Here are a few more examples:
// 
// * 
// 
// ########################
// #...............b.C.D.f#
// #.######################
// #.....@.a.B.c.d.A.e.F.g#
// ########################
// 
// Shortest path is 132 steps: b, a, c, d, f, e, g
// 
// * 
// 
// #################
// #i.G..c...e..H.p#
// ########.########
// #j.A..b...f..D.o#
// ########@########
// #k.E..a...g..B.n#
// ########.########
// #l.F..d...h..C.m#
// #################
// 
// Shortest paths are 136 steps;
// one is: a, f, b, j, g, n, h, d, l, o, e, p, c, i, k, m
// 
// * 
// 
// ########################
// #@..............ac.GI.b#
// ###d#e#f################
// ###A#B#C################
// ###g#h#i################
// ########################
// 
// Shortest paths are 81 steps; one is: a, c, f, i, d, g, b, e, h
// 
// How many steps is the shortest path that collects all of the keys?
//
// --- Part Two ---
// ----------------
// 
// You arrive at the vault only to discover that there is not one vault,
// but four - each with its own entrance.
// 
// On your map, find the area in the middle that looks like this:
// 
// ...
// .@.
// ...
// 
// Update your map to instead use the correct data:
// 
// @#@
// ###
// @#@
// 
// This change will split your map into four separate sections, each with
// its own entrance:
// 
// #######       #######
// #a.#Cd#       #a.#Cd#
// ##...##       ##   @#@   ##
// ##.@.##  -->  ##   ###   ##
// ##...##       ##   @#@   ##
// #cB#Ab#       #cB#Ab#
// #######       #######
// 
// Because some of the keys are for doors in other vaults, it would take
// much too long to collect all of the keys by yourself. Instead, you
// deploy four remote-controlled robots. Each starts at one of the
// entrances (@).
// 
// Your goal is still to collect all of the keys in the fewest steps, but
// now, each robot has its own position and can move independently. You
// can only remotely control a single robot at a time. Collecting a key
// instantly unlocks any corresponding doors, regardless of the vault in
// which the key or door is found.
// 
// For example, in the map above, the top-left robot first collects key a,
// unlocking door A in the bottom-right vault:
// 
// #######
// #@.#Cd#
// ##.#@##
// #######
// ##@#@##
// #cB#.b#
// #######
// 
// Then, the bottom-right robot collects key b, unlocking door B in the
// bottom-left vault:
// 
// #######
// #@.#Cd#
// ##.#@##
// #######
// ##@#.##
// #c.#.@#
// #######
// 
// Then, the bottom-left robot collects key c:
// 
// #######
// #@.#.d#
// ##.#@##
// #######
// ##.#.##
// #@.#.@#
// #######
// 
// Finally, the top-right robot collects key d:
// 
// #######
// #@.#.@#
// ##.#.##
// #######
// ##.#.##
// #@.#.@#
// #######
// 
// In this example, it only took 8 steps to collect all of the keys.
// 
// Sometimes, multiple robots might have keys available, or a robot might
// have to wait for multiple keys to be collected:
// 
// ###############
// #d.ABC.#.....a#
// ######@#@######
// ###############
// ######@#@######
// #b.....#.....c#
// ###############
// 
// First, the top-right, bottom-left, and bottom-right robots take turns
// collecting keys a, b, and c, a total of 6 + 6 + 6 = 18 steps. Then,
// the top-left robot can access key d, spending another 6 steps;
// collecting all of the keys here takes a minimum of 24 steps.
// 
// Here's a more complex example:
// 
// #############
// #DcBa.#.GhKl#
// #.###@#@#I###
// #e#d#####j#k#
// ###C#@#@###J#
// #fEbA.#.FgHi#
// #############     
// 
// * Top-left robot collects key a.
// 
// * Bottom-left robot collects key b.
// 
// * Top-left robot collects key c.
// 
// * Bottom-left robot collects key d.
// 
// * Top-left robot collects key e.
// 
// * Bottom-left robot collects key f.
// 
// * Bottom-right robot collects key g.
// 
// * Top-right robot collects key h.
// 
// * Bottom-right robot collects key i.
// 
// * Top-right robot collects key j.
// 
// * Bottom-right robot collects key k.
// 
// * Top-right robot collects key l.
// 
// In the above example, the fewest steps to collect all of the keys is
// 32.
// 
// Here's an example with more choices:
// 
// #############
// #g#f.D#..h#l#
// #F###e#E###.#
// #dCba@#@BcIJ#
// #############
// #nK.L@#@G...#
// #M###N#H###.#
// #o#m..#i#jk.#
// #############
// 
// One solution with the fewest steps is:
// 
// * Top-left robot collects key e.
// 
// * Top-right robot collects key h.
// 
// * Bottom-right robot collects key i.
// 
// * Top-left robot collects key a.
// 
// * Top-left robot collects key b.
// 
// * Top-right robot collects key c.
// 
// * Top-left robot collects key d.
// 
// * Top-left robot collects key f.
// 
// * Top-left robot collects key g.
// 
// * Bottom-right robot collects key k.
// 
// * Bottom-right robot collects key j.
// 
// * Top-right robot collects key l.
// 
// * Bottom-left robot collects key n.
// 
// * Bottom-left robot collects key m.
// 
// * Bottom-left robot collects key o.
// 
// This example requires at least 72 steps to collect all keys.
// 
// After updating your map and using the remote-controlled robots, what
// is the fewest steps necessary to collect all of the keys?


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
#include "absl/log/log.h"
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
