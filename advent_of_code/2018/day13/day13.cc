#include "advent_of_code/2018/day13/day13.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Cart {
  Point pos;
  Point dir;
  int turns;
  bool to_remove = false;
  bool operator<(const Cart& o) const {
    if (pos.y != o.pos.y) return pos.y < o.pos.y;
    return pos.x < o.pos.x;
  }
};

struct State {
  CharBoard board;
  std::vector<Cart> carts;

  std::string DebugString() {
    CharBoard tmp = board;
    for (const Cart& c : carts) {
      if (c.dir == Cardinal::kNorth)
        tmp[c.pos] = '^';
      else if (c.dir == Cardinal::kSouth)
        tmp[c.pos] = 'v';
      else if (c.dir == Cardinal::kWest)
        tmp[c.pos] = '<';
      else if (c.dir == Cardinal::kEast)
        tmp[c.pos] = '>';
      else
        tmp[c.pos] = '?';
    }
    return tmp.DebugString();
  }

  absl::StatusOr<absl::optional<Point>> RunStep() {
    absl::optional<Point> ret;
    absl::flat_hash_set<Point> cart_locations;
    std::sort(carts.begin(), carts.end());
    for (const Cart& c : carts) cart_locations.insert(c.pos);
    for (Cart& c : carts) {
      cart_locations.erase(c.pos);
      c.pos += c.dir;
      if (!board.OnBoard(c.pos)) return AdventDay::Error("Off board");
      if (cart_locations.contains(c.pos)) {
        // if (ret) return AdventDay::Error("duplicate crash");
        ret = c.pos;
        for (Cart& c2 : carts) {
          if (c.pos == c2.pos) {
            c2.to_remove = true;
          }
        }
      } else {
        cart_locations.insert(c.pos);
      }
      switch (board[c.pos]) {
        case '-': {
          if (c.dir != Cardinal::kWest && c.dir != Cardinal::kEast) {
            return AdventDay::Error("Bad dir on '-'");
          }
          break;
        }
        case '|': {
          if (c.dir != Cardinal::kNorth && c.dir != Cardinal::kSouth) {
            return AdventDay::Error("Bad dir on '-'");
          }
          break;
        }
        case '/': {
          if (c.dir == Cardinal::kEast)
            c.dir = Cardinal::kNorth;
          else if (c.dir == Cardinal::kNorth)
            c.dir = Cardinal::kEast;
          else if (c.dir == Cardinal::kSouth)
            c.dir = Cardinal::kWest;
          else if (c.dir == Cardinal::kWest)
            c.dir = Cardinal::kSouth;
          else
            return AdventDay::Error("Bad dir on '/'");
          break;
        }
        case '\\': {
          if (c.dir == Cardinal::kEast)
            c.dir = Cardinal::kSouth;
          else if (c.dir == Cardinal::kSouth)
            c.dir = Cardinal::kEast;
          else if (c.dir == Cardinal::kNorth)
            c.dir = Cardinal::kWest;
          else if (c.dir == Cardinal::kWest)
            c.dir = Cardinal::kNorth;
          else
            return AdventDay::Error("Bad dir on '\\'");
          break;
        }
        case '+': {
          switch (c.turns % 3) {
            case 0:
              c.dir = c.dir.rotate_left();
              break;
            case 1:
              // Straight through. No dir change.
              break;
            case 2:
              c.dir = c.dir.rotate_right();
              break;
            default:
              return AdventDay::Error("Bad mod");
          }
          ++c.turns;
          break;
        }
        default:
          return AdventDay::Error("Bad board value");
      }
    }
    if (ret) {
      std::vector<Cart> new_carts;
      for (const Cart& c : carts) {
        if (!c.to_remove) new_carts.push_back(c);
      }
      carts = std::move(new_carts);
    }
    return ret;
  }
};

absl::StatusOr<State> FindKarts(const CharBoard& b) {
  State state{.board = b};
  for (Point p : state.board.range()) {
    switch (state.board[p]) {
      case ' ':
      case '|':
      case '-':
      case '/':
      case '\\':
      case '+': {
        // All good.
        break;
      }
      case '^': {
        state.carts.push_back({.pos = p, .dir = Cardinal::kNorth, .turns = 0});
        state.board[p] = '|';
        break;
      }
      case 'v': {
        state.carts.push_back({.pos = p, .dir = Cardinal::kSouth, .turns = 0});
        state.board[p] = '|';
        break;
      }
      case '<': {
        state.carts.push_back({.pos = p, .dir = Cardinal::kWest, .turns = 0});
        state.board[p] = '-';
        break;
      }
      case '>': {
        state.carts.push_back({.pos = p, .dir = Cardinal::kEast, .turns = 0});
        state.board[p] = '-';
        break;
      }
      default:
        return AdventDay::Error("Bad char @", p.DebugString());
    }
  }
  return state;
}

}  // namespace

absl::StatusOr<std::vector<std::string>> Day13_2018::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.empty()) return Error("bad input");
  if (RE2::PartialMatch(input[0], "^HACK:")) input = input.subspan(1);
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  absl::StatusOr<State> state = FindKarts(*b);
  if (!state.ok()) return state.status();

  while (true) {
    VLOG(1) << "State:\n" << state->DebugString();
    absl::StatusOr<absl::optional<Point>> collision = state->RunStep();
    if (!collision.ok()) return collision.status();
    if (*collision)
      return std::vector<std::string>{(*collision)->DebugString()};
  }

  return Error("Left infinite loop");
}

absl::StatusOr<std::vector<std::string>> Day13_2018::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.empty()) return Error("bad input");
  if (RE2::PartialMatch(input[0], "^HACK:")) input = input.subspan(1);
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  absl::StatusOr<State> state = FindKarts(*b);
  if (!state.ok()) return state.status();

  while (state->carts.size() > 1) {
    VLOG(1) << "State:\n" << state->DebugString();
    absl::StatusOr<absl::optional<Point>> collision = state->RunStep();
    if (!collision.ok()) return collision.status();
  }

  return std::vector<std::string>{state->carts[0].pos.DebugString()};
}

}  // namespace advent_of_code
