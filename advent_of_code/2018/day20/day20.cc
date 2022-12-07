// https://adventofcode.com/2018/day/20
//
// --- Day 20: A Regular Map ---
// -----------------------------
// 
// While you were learning about instruction pointers, the Elves made
// considerable progress. When you look up, you discover that the North
// Pole base construction project has completely surrounded you.
// 
// The area you are in is made up entirely of rooms and doors. The rooms
// are arranged in a grid, and rooms only connect to adjacent rooms when
// a door is present between them.
// 
// For example, drawing rooms as ., walls as #, doors as | or -, your
// current position as X, and where north is up, the area you're in might
// look like this:
// 
// #####
// #.|.#
// #-###
// #.|X#
// #####
// 
// You get the attention of a passing construction Elf and ask for a map.
// "I don't have time to draw out a map of this place - it's huge.
// Instead, I can give you directions to every room in the facility!" He
// writes down some directions on a piece of parchment and runs off. In
// the example above, the instructions might have been ^WNE$, a regular
// expression or "regex" (your puzzle input).
// 
// The regex matches routes (like WNE for "west, north, east") that will
// take you from your current room through various doors in the facility.
// In aggregate, the routes will take you through every door in the
// facility at least once; mapping out all of these routes will let you
// build a proper map and find your way around.
// 
// ^ and $ are at the beginning and end of your regex; these just mean
// that the regex doesn't match anything outside the routes it describes.
// (Specifically, ^ matches the start of the route, and $ matches the end
// of it.) These characters will not appear elsewhere in the regex.
// 
// The rest of the regex matches various sequences of the characters N
// (north), S (south), E (east), and W (west). In the example above,
// ^WNE$ matches only one route, WNE, which means you can move west, then
// north, then east from your current position. Sequences of letters like
// this always match that exact route in the same order.
// 
// Sometimes, the route can branch. A branch is given by a list of
// options separated by pipes (|) and wrapped in parentheses. So,
// ^N(E|W)N$ contains a branch: after going north, you must choose to go
// either east or west before finishing your route by going north again.
// By tracing out the possible routes after branching, you can determine
// where the doors are and, therefore, where the rooms are in the
// facility.
// 
// For example, consider this regex: ^ENWWW(NEEE|SSE(EE|N))$
// 
// This regex begins with ENWWW, which means that from your current
// position, all routes must begin by moving east, north, and then west
// three times, in that order. After this, there is a branch. Before you
// consider the branch, this is what you know about the map so far, with
// doors you aren't sure about marked with a ?:
// 
// #?#?#?#?#
// ?.|.|.|.?
// #?#?#?#-#
// ?X|.?
// #?#?#
// 
// After this point, there is (NEEE|SSE(EE|N)). This gives you exactly
// two options: NEEE and SSE(EE|N). By following NEEE, the map now looks
// like this:
// 
// #?#?#?#?#
// ?.|.|.|.?
// #-#?#?#?#
// ?.|.|.|.?
// #?#?#?#-#
// ?X|.?
// #?#?#
// 
// Now, only SSE(EE|N) remains. Because it is in the same parenthesized
// group as NEEE, it starts from the same room NEEE started in. It states
// that starting from that point, there exist doors which will allow you
// to move south twice, then east; this ends up at another branch. After
// that, you can either move east twice or north once. This information
// fills in the rest of the doors:
// 
// #?#?#?#?#
// ?.|.|.|.?
// #-#?#?#?#
// ?.|.|.|.?
// #-#?#?#-#
// ?.?.?X|.?
// #-#-#?#?#
// ?.|.|.|.?
// #?#?#?#?#
// 
// Once you've followed all possible routes, you know the remaining
// unknown parts are all walls, producing a finished map of the facility:
// 
// #########
// #.|.|.|.#
// #-#######
// #.|.|.|.#
// #-#####-#
// #.#.#X|.#
// #-#-#####
// #.|.|.|.#
// #########
// 
// Sometimes, a list of options can have an empty option, like
// (NEWS|WNSE|). This means that routes at this point could effectively
// skip the options in parentheses and move on immediately. For example,
// consider this regex and the corresponding map:
// 
// ^ENNWSWW(NEWS|)SSSEEN(WNSE|)EE(SWEN|)NNN$
// 
// ###########
// #.|.#.|.#.#
// #-###-#-#-#
// #.|.|.#.#.#
// #-#####-#-#
// #.#.#X|.#.#
// #-#-#####-#
// #.#.|.|.|.#
// #-###-###-#
// #.|.|.#.|.#
// ###########
// 
// This regex has one main route which, at three locations, can
// optionally include additional detours and be valid: (NEWS|), (WNSE|),
// and (SWEN|). Regardless of which option is taken, the route continues
// from the position it is left at after taking those steps. So, for
// example, this regex matches all of the following routes (and more that
// aren't listed here):
// 
// * ENNWSWWSSSEENEENNN
// 
// * ENNWSWWNEWSSSSEENEENNN
// 
// * ENNWSWWNEWSSSSEENEESWENNNN
// 
// * ENNWSWWSSSEENWNSEEENNN
// 
// By following the various routes the regex matches, a full map of all
// of the doors and rooms in the facility can be assembled.
// 
// To get a sense for the size of this facility, you'd like to determine
// which room is furthest from you: specifically, you would like to find
// the room for which the shortest path to that room would require
// passing through the most doors.
// 
// * In the first example (^WNE$), this would be the north-east corner
// 3 doors away.
// 
// * In the second example (^ENWWW(NEEE|SSE(EE|N))$), this would be the
// south-east corner 10 doors away.
// 
// * In the third example (^ENNWSWW(NEWS|)SSSEEN(WNSE|)EE(SWEN|)NNN$),
// this would be the north-east corner 18 doors away.
// 
// Here are a few more examples:
// 
// Regex: ^ESSWWN(E|NNENN(EESS(WNSE|)SSS|WWWSSSSE(SW|NNNE)))$
// Furthest room requires passing 23 doors
// 
// #############
// #.|.|.|.|.|.#
// #-#####-###-#
// #.#.|.#.#.#.#
// #-#-###-#-#-#
// #.#.#.|.#.|.#
// #-#-#-#####-#
// #.#.#.#X|.#.#
// #-#-#-###-#-#
// #.|.#.|.#.#.#
// ###-#-###-#-#
// #.|.#.|.|.#.#
// #############
// 
// Regex: ^WSSEESWWWNW(S|NENNEEEENN(ESSSSW(NWSW|SSEN)|WSWWN(E|WWS(E|SS))))$
// Furthest room requires passing 31 doors
// 
// ###############
// #.|.|.|.#.|.|.#
// #-###-###-#-#-#
// #.|.#.|.|.#.#.#
// #-#########-#-#
// #.#.|.|.|.|.#.#
// #-#-#########-#
// #.#.#.|X#.|.#.#
// ###-#-###-#-#-#
// #.|.#.#.|.#.|.#
// #-###-#####-###
// #.|.#.|.|.#.#.#
// #-#-#####-#-#-#
// #.#.|.|.|.#.|.#
// ###############
// 
// What is the largest number of doors you would be required to pass
// through to reach a room? That is, find the room for which the shortest
// path from your starting location to that room would require passing
// through the most doors; what is the fewest doors you can pass through
// to reach it?
//
// --- Part Two ---
// ----------------
// 
// Okay, so the facility is big.
// 
// How many rooms have a shortest path from your current location that
// pass through at least 1000 doors?


#include "advent_of_code/2018/day20/day20.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct ParsedRE {
  enum Type {
    kLiteral = 1,
    kBranch = 2,
    kSequence = 3,
  } type;
  absl::string_view val;
  std::vector<std::unique_ptr<ParsedRE>> children;

  std::string DebugString(std::string prefix = "") {
    if (type == kLiteral) return prefix.append(std::string(val));
    std::string sub_prefix = prefix;
    sub_prefix.append(2, ' ');
    return absl::StrCat(
        prefix, type == kBranch ? "<branch>\n" : "<sequence\n",
        absl::StrJoin(
            children, "\n",
            [sub_prefix](std::string* out, const std::unique_ptr<ParsedRE>& p) {
              absl::StrAppend(out, p->DebugString(sub_prefix));
            }));
  }
};

std::vector<absl::string_view> Tokenize(absl::string_view re) {
  std::vector<absl::string_view> ret;
  int tok_start = 0;
  for (int i = 0; i < re.size(); ++i) {
    if (re[i] == '(' || re[i] == ')' || re[i] == '|') {
      ret.push_back(re.substr(tok_start, i - tok_start));
      ret.push_back(re.substr(i, 1));
      tok_start = i + 1;
    }
  }
  ret.push_back(re.substr(tok_start, re.size() - tok_start));
  return ret;
}

absl::StatusOr<ParsedRE> Parse(absl::string_view re) {
  std::vector<absl::string_view> tokens = Tokenize(re);
  ParsedRE ret;
  ret.type = ParsedRE::kSequence;
  std::vector<ParsedRE*> stack = {&ret};
  for (absl::string_view tok : tokens) {
    if (stack.empty()) return Error("Empty stack");
    if (tok == "(") {
      auto next = absl::make_unique<ParsedRE>();
      next->type = ParsedRE::kBranch;
      stack.back()->children.push_back(std::move(next));
      stack.push_back(stack.back()->children.back().get());
      next = absl::make_unique<ParsedRE>();
      next->type = ParsedRE::kSequence;
      stack.back()->children.push_back(std::move(next));
      stack.push_back(stack.back()->children.back().get());
    } else if (tok == ")") {
      if (stack.back()->type != ParsedRE::kSequence) {
        return Error("Bad type at ')'");
      }
      stack.pop_back();
      if (stack.empty()) return Error("Empty stack");
      if (stack.back()->type != ParsedRE::kBranch) {
        return Error("Bad type at ')' (2)");
      }
      stack.pop_back();
    } else if (tok == "|") {
      if (stack.back()->type != ParsedRE::kSequence) {
        return Error("Bad type at ')'");
      }
      stack.pop_back();
      if (stack.empty()) return Error("Empty stack");
      auto next = absl::make_unique<ParsedRE>();
      next->type = ParsedRE::kSequence;
      stack.back()->children.push_back(std::move(next));
      stack.push_back(stack.back()->children.back().get());
    } else {
      auto next = absl::make_unique<ParsedRE>();
      next->type = ParsedRE::kLiteral;
      next->val = tok;
      stack.back()->children.push_back(std::move(next));
    }
  }
  if (stack.size() != 1) return Error("Bad parse");
  if (stack.back() != &ret) return Error("Bad parse 2");
  return ret;
}

absl::StatusOr<absl::flat_hash_set<Point>> WalkAllPaths(
    const ParsedRE& re, Point cur, absl::flat_hash_set<Point>* sparse_board) {
  absl::flat_hash_set<Point> ret;
  switch (re.type) {
    case ParsedRE::kLiteral: {
      if (re.children.size() != 0) {
        return Error("Literal with non-empty children");
      }
      for (char c : re.val) {
        Point dir;
        switch (c) {
          case 'N':
            dir = Cardinal::kNorth;
            break;
          case 'S':
            dir = Cardinal::kSouth;
            break;
          case 'W':
            dir = Cardinal::kWest;
            break;
          case 'E':
            dir = Cardinal::kEast;
            break;
          default:
            return Error("Bad direction in: ", re.val);
        }
        // Advance twice to include door.
        cur += dir;
        sparse_board->insert(cur);
        cur += dir;
        sparse_board->insert(cur);
      }
      ret = {cur};
      break;
    }
    case ParsedRE::kSequence: {
      if (!re.val.empty()) {
        return Error("Sequence with non-empty value");
      }
      ret = {cur};
      for (const auto& child : re.children) {
        absl::flat_hash_set<Point> next;
        for (Point p : ret) {
          absl::StatusOr<absl::flat_hash_set<Point>> this_next =
              WalkAllPaths(*child, p, sparse_board);
          if (!this_next.ok()) return this_next.status();
          for (Point p : *this_next) next.insert(p);
        }
        ret = std::move(next);
      }
      break;
    }
    case ParsedRE::kBranch: {
      if (!re.val.empty()) {
        return Error("Branch with non-empty value");
      }
      ret = {};
      for (const auto& child : re.children) {
        absl::StatusOr<absl::flat_hash_set<Point>> this_next =
            WalkAllPaths(*child, cur, sparse_board);
        if (!this_next.ok()) return this_next.status();
        for (Point p : *this_next) ret.insert(p);
      }
      break;
    }
    default:
      return Error("Bad type: ", re.type);
  }
  return ret;
}

absl::StatusOr<CharBoard> ConstructRoom(absl::string_view re,
                                        Point* start_ret) {
  Point start = {0, 0};

  absl::StatusOr<ParsedRE> parsed_re = Parse(re);
  if (!parsed_re.ok()) return parsed_re.status();

  VLOG(1) << parsed_re->DebugString();

  absl::flat_hash_set<Point> sparse_board = {start};
  absl::StatusOr<absl::flat_hash_set<Point>> final_points =
      WalkAllPaths(*parsed_re, start, &sparse_board);
  if (!final_points.ok()) return final_points.status();

  PointRectangle grid = {start, start};
  for (Point p : sparse_board) {
    VLOG(2) << "Point @" << p;
    grid.ExpandInclude(p);
  }

  CharBoard board(grid.max.x - grid.min.x + 1 + 2,
                  grid.max.y - grid.min.y + 1 + 2);
  for (Point p : board.range()) {
    Point test_point = p + grid.min + Point{-1, -1};
    if (sparse_board.contains(test_point)) {
      if (test_point.x % 2 == 0 && test_point.y % 2 == 0) {
        // Room.
        board[p] = '.';
      } else if (test_point.x % 2 && test_point.y % 2 == 0) {
        board[p] = '|';
      } else if (test_point.x % 2 == 0 && test_point.y % 2) {
        board[p] = '-';
      } else {
        return Error("Sparse board contains corner: ", test_point);
      }
    } else {
      board[p] = '#';
    }
  }
  *start_ret = -grid.min + Point{1, 1};
  return board;
}

class RoomWalk : public BFSInterface<RoomWalk, Point> {
 public:
  RoomWalk(const CharBoard& b, Point start, int* max_dist)
      : board_(b), max_dist_(max_dist), cur_(start) {}

  Point identifier() const override { return cur_; }
  bool IsFinal() const override { return false; }

  void AddNextSteps(State* state) const override {
    *max_dist_ = std::max(num_steps(), *max_dist_);

    for (Point dir : Cardinal::kFourDirs) {
      Point door_p = cur_ + dir;
      if (board_.OnBoard(door_p) &&
          (board_[door_p] == '|' || board_[door_p] == '-')) {
        RoomWalk next = *this;
        next.cur_ = cur_ + 2 * dir;
        state->AddNextStep(next);
      }
    }
  }

 private:
  const CharBoard& board_;
  int* max_dist_;
  Point cur_;
};

class RoomWalkPast : public BFSInterface<RoomWalkPast, Point> {
 public:
  RoomWalkPast(const CharBoard& b, Point start, int min_dist, int* count)
      : board_(b), min_dist_(min_dist), count_(count), cur_(start) {
    *count_ = 0;
  }

  Point identifier() const override { return cur_; }
  bool IsFinal() const override { return false; }

  void AddNextSteps(State* state) const override {
    if (num_steps() >= min_dist_) ++*count_;

    for (Point dir : Cardinal::kFourDirs) {
      Point door_p = cur_ + dir;
      if (board_.OnBoard(door_p) &&
          (board_[door_p] == '|' || board_[door_p] == '-')) {
        RoomWalkPast next = *this;
        next.cur_ = cur_ + 2 * dir;
        state->AddNextStep(next);
      }
    }
  }

 private:
  const CharBoard& board_;
  int min_dist_;
  int* count_;
  Point cur_;
};

}  // namespace

absl::StatusOr<std::string> Day_2018_20::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  absl::string_view re = input[0];
  if (re[0] != '^') return Error("No front anchor: ", re);
  re = re.substr(1);
  if (re[re.size() - 1] != '$') return Error("No back anchor");
  re = re.substr(0, re.size() - 1);

  Point start;
  absl::StatusOr<CharBoard> room = ConstructRoom(re, &start);
  if (!room.ok()) return room.status();

  VLOG(1) << "Start @" << start << " in Room:\n" << *room;

  int max_path = -1;
  absl::optional<int> null_dist =
      RoomWalk(*room, start, &max_path).FindMinSteps();
  if (null_dist) return Error("Path walk terminated?!?");

  return IntReturn(max_path);
}

absl::StatusOr<std::string> Day_2018_20::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  absl::string_view re = input[0];
  if (re[0] != '^') return Error("No front anchor: ", re);
  re = re.substr(1);
  if (re[re.size() - 1] != '$') return Error("No back anchor");
  re = re.substr(0, re.size() - 1);

  Point start;
  absl::StatusOr<CharBoard> room = ConstructRoom(re, &start);
  if (!room.ok()) return room.status();

  VLOG(1) << "Start @" << start << " in Room:\n" << *room;

  int count = -1;
  absl::optional<int> null_dist =
      RoomWalkPast(*room, start, 1000, &count).FindMinSteps();
  if (null_dist) return Error("Path walk terminated?!?");

  return IntReturn(count);
}

}  // namespace advent_of_code
