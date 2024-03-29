#include "advent_of_code/2018/day13/day13.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
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
    return absl::StrCat(tmp);
  }

  absl::StatusOr<std::optional<Point>> RunStep() {
    std::optional<Point> ret;
    absl::flat_hash_set<Point> cart_locations;
    std::sort(carts.begin(), carts.end());
    for (const Cart& c : carts) cart_locations.insert(c.pos);
    for (Cart& c : carts) {
      cart_locations.erase(c.pos);
      c.pos += c.dir;
      if (!board.OnBoard(c.pos)) return Error("Off board");
      if (cart_locations.contains(c.pos)) {
        // if (ret) return Error("duplicate crash");
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
            return Error("Bad dir on '-'");
          }
          break;
        }
        case '|': {
          if (c.dir != Cardinal::kNorth && c.dir != Cardinal::kSouth) {
            return Error("Bad dir on '-'");
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
            return Error("Bad dir on '/'");
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
            return Error("Bad dir on '\\'");
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
              return Error("Bad mod");
          }
          ++c.turns;
          break;
        }
        default:
          return Error("Bad board value");
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
        return Error("Bad char @", p);
    }
  }
  return state;
}

}  // namespace

absl::StatusOr<std::string> Day_2018_13::Part1(
    absl::Span<std::string_view> input) const {
  if (input.empty()) return Error("bad input");
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  ASSIGN_OR_RETURN(State state, FindKarts(b));

  while (true) {
    VLOG(1) << "State:\n" << state.DebugString();
    ASSIGN_OR_RETURN(std::optional<Point> collision, state.RunStep());
    if (collision) return AdventReturn(collision);
  }

  return Error("Left infinite loop");
}

absl::StatusOr<std::string> Day_2018_13::Part2(
    absl::Span<std::string_view> input) const {
  if (input.empty()) return Error("bad input");
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  ASSIGN_OR_RETURN(State state, FindKarts(b));

  while (state.carts.size() > 1) {
    VLOG(1) << "State:\n" << state.DebugString();
    ASSIGN_OR_RETURN(std::optional<Point> collision, state.RunStep());
    (void)collision;
  }

  return AdventReturn(state.carts[0].pos);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2018, /*day=*/13,
    []() { return std::unique_ptr<AdventDay>(new Day_2018_13()); });

}  // namespace advent_of_code
