#include "advent_of_code/2018/day15/day15.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point_walk.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

constexpr Point kOrderedDirs[] = {Cardinal::kNorth, Cardinal::kWest,
                                  Cardinal::kEast, Cardinal::kSouth};

struct PointAndDistance {
  int d = std::numeric_limits<int>::max();
  Point p;
  bool operator<(const PointAndDistance& o) const {
    if (d != o.d) return d < o.d;
    return PointYThenXLT()(p, o.p);
  }
};

class FindEnemyAdjacent : public BFSInterface<FindEnemyAdjacent, Point> {
 public:

  FindEnemyAdjacent(const CharBoard& b, Point start, char find,
                    PointAndDistance* ret)
      : board_(b), cur_(start), ret_(ret), find_(find) {}

  Point identifier() const override { return cur_; }

  bool IsFinal() const override { return false; }

  void AddNextSteps(State* state) const override {
    // Stop exploring if we've found a match.
    if (num_steps() > ret_->d) return;

    for (Point dir : kOrderedDirs) {
      Point next = cur_ + dir;
      if (!board_.OnBoard(next)) continue;
      if (board_[next] == '.') {
        FindEnemyAdjacent add = *this;
        add.cur_ = next;
        state->AddNextStep(add);
      } else if (board_[next] == find_) {
        PointAndDistance test{num_steps(), cur_};
        if (test < *ret_) {
          *ret_ = test;
        }
      }
    }
  }

 private:
  const CharBoard& board_;
  Point cur_;
  PointAndDistance* ret_;
  char find_;
};

class GameBoard {
 public:
  explicit GameBoard(CharBoard board, int elf_attack = 3)
      : board_(std::move(board)), elf_attack_(elf_attack) {
    for (Point p : board_.range()) {
      if (board_[p] == 'G' || board_[p] == 'E') {
        hit_points_[p] = 200;
      }
    }
  }

  int CountElves() {
    int count = 0;
    for (const auto& [p, _] : hit_points_) {
      if (board_[p] == 'E') ++count;
    }
    return count;
  }

  int rounds() { return rounds_; }
  int TotalHitPoints() {
    int total_hp = 0;
    for (const auto& [_, hp] : hit_points_) total_hp += hp;
    return total_hp;
  }

  absl::StatusOr<Point> TryAttack(Point p) {
    char find = '\0';
    int attack;
    if (board_[p] == 'G') {
      find = 'E';
      attack = 3;
    } else if (board_[p] == 'E') {
      find = 'G';
      attack = elf_attack_;
    } else
      return Error("HP at bad location (attack): ", p);
    int fewest_hp = std::numeric_limits<int>::max();
    Point fewest_hp_location;
    for (Point dir : kOrderedDirs) {
      Point check = p + dir;
      if (board_.OnBoard(check) && board_[check] == find) {
        auto it = hit_points_.find(check);
        if (it == hit_points_.end()) return Error("No HP at location");
        if (it->second < fewest_hp) {
          fewest_hp = it->second;
          fewest_hp_location = check;
        }
      }
    }
    if (fewest_hp == std::numeric_limits<int>::max()) {
      return Point{-1, -1};
    }

    auto it = hit_points_.find(fewest_hp_location);
    if (it == hit_points_.end()) return Error("No HP at location");
    if (it->second <= attack) {
      VLOG(1) << "Opponent " << find << " @" << fewest_hp_location << " died";
      // Opponent died.
      board_[fewest_hp_location] = '.';
      hit_points_.erase(it);
      return fewest_hp_location;
    }

    hit_points_[fewest_hp_location] -= attack;
    return Point{-1, -1};
  }

  absl::StatusOr<Point> TryMove(Point p) {
    char find = '\0';
    if (board_[p] == 'G')
      find = 'E';
    else if (board_[p] == 'E')
      find = 'G';
    else
      return Error("HP at bad location (move): ", p);

    PointAndDistance p_and_d;
    FindEnemyAdjacent(board_, p, find, &p_and_d).Walk();
    Point move_to = p;
    if (p_and_d.d == 0) return move_to;
    if (p_and_d.d == std::numeric_limits<int>::max()) return move_to;

    VLOG(1) << "Nearest enemy from " << board_[p] << " @" << p << " is "
            << p_and_d.d << " away adjacent-to-" << p_and_d.p;
    int min_path_length = std::numeric_limits<int>::max();
    for (Point dir : kOrderedDirs) {
      Point check = p + dir;
      if (!board_.OnBoard(check) || board_[check] != '.') continue;
      absl::optional<int> dist = PointWalk({
        .start = check,
        .is_good = [&](Point test, int) {
          return board_.OnBoard(test) && board_[test] == '.';
        },
        .is_final = [&](Point test, int) {
          return test == p_and_d.p;
        }
      }).FindMinSteps();
      if (dist && *dist < min_path_length) {
        min_path_length = *dist;
        move_to = check;
      }
    }
    if (min_path_length != p_and_d.d - 1) {
      return Error("Distance integrity check: ", min_path_length,
                   " != ", p_and_d.d - 1);
    }
    if (move_to == p) {
      return Error("Can't find path from ", p, " to ", p_and_d.p);
    }
    VLOG(1) << "Moving " << board_[p] << " from " << p << " to " << move_to;
    board_[move_to] = board_[p];
    board_[p] = '.';
    hit_points_[move_to] = hit_points_[p];
    hit_points_.erase(p);

    return move_to;
  }

  absl::StatusOr<bool> RunStep() {
    int done = false;

    std::vector<Point> actors;
    for (const auto& [p, _] : hit_points_) actors.push_back(p);
    absl::flat_hash_set<Point> dead;
    for (Point p : actors) {
      if (dead.contains(p)) continue;
      if (hit_points_.find(p) == hit_points_.end()) {
        return Error("Integrity check (actor)");
      }
      if (!EnemyExists()) {
        done = true;
        break;
      }
      absl::StatusOr<Point> moved = TryMove(p);
      if (!moved.ok()) return moved.status();
      absl::StatusOr<Point> died_at = TryAttack(*moved);
      if (!died_at.ok()) return died_at.status();
      if (*died_at != Point{-1, -1}) {
        dead.insert(*died_at);
      }
    }

    if (!done) ++rounds_;
    return done;
  }

  bool EnemyExists() {
    absl::flat_hash_set<char> types;
    for (const auto& [p, _] : hit_points_) {
      types.insert(board_[p]);
    }
    return types.size() == 2;
  }

  std::string DebugString() {
    return absl::StrCat(
        absl::StrJoin(hit_points_, "",
                      [](std::string* out, const std::pair<Point, int>& pair) {
                        absl::StrAppend(out, "  ", pair.second, " @",
                                        pair.first, "\n");
                      }),
        board_);
  }

 private:
  CharBoard board_;
  int rounds_ = 0;
  std::map<Point, int, PointYThenXLT> hit_points_;
  int elf_attack_;
};

}  // namespace

absl::StatusOr<std::string> Day_2018_15::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  GameBoard game(std::move(*b));
  bool done = false;
  while (!done) {
    VLOG(1) << "State: [" << game.rounds() << "]";
    VLOG(2) << "\n" << game.DebugString();
    absl::StatusOr<bool> game_ended = game.RunStep();
    if (!game_ended.ok()) return game_ended.status();
    done = *game_ended;
  }

  VLOG(1) << "State: [" << game.rounds() << "]\n" << game.DebugString();

  return AdventReturn(game.TotalHitPoints() * game.rounds());
}

absl::StatusOr<std::string> Day_2018_15::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  for (int elf_attack = 3; true; ++elf_attack) {
    // TODO(@monkeynova): Binary search.
    GameBoard game(std::move(*b), elf_attack);
    int start_elves = game.CountElves();
    bool done = false;
    while (!done) {
      VLOG(1) << "State: [" << game.rounds() << "]";
      VLOG(2) << "\n" << game.DebugString();
      absl::StatusOr<bool> game_ended = game.RunStep();
      if (!game_ended.ok()) return game_ended.status();
      done = *game_ended;
    }
    int end_elves = game.CountElves();
    VLOG(1) << "elf_attack: " << elf_attack << ": " << start_elves << " => "
            << end_elves;
    if (start_elves == end_elves) {
      VLOG(1) << "State: [" << game.rounds() << "]\n" << game.DebugString();
      return AdventReturn(game.TotalHitPoints() * game.rounds());
    }
  }

  return Error("Left infinite loop");
}

}  // namespace advent_of_code
