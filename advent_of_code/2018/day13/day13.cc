// https://adventofcode.com/2018/day/13
//
// --- Day 13: Mine Cart Madness ---
// ---------------------------------
// 
// A crop of this size requires significant logistics to transport
// produce, soil, fertilizer, and so on. The Elves are very busy pushing
// things around in carts on some kind of rudimentary system of tracks
// they've come up with.
// 
// Seeing as how cart-and-track systems don't appear in recorded history
// for another 1000 years, the Elves seem to be making this up as they go
// along. They haven't even figured out how to avoid collisions yet.
// 
// You map out the tracks (your puzzle input) and see where you can help.
// 
// Tracks consist of straight paths (| and -), curves (/ and \), and
// intersections (+). Curves connect exactly two perpendicular pieces of
// track; for example, this is a closed loop:
// 
// /----\
// |    |
// |    |
// \----/
// 
// Intersections occur when two perpendicular paths cross. At an
// intersection, a cart is capable of turning left, turning right, or
// continuing straight. Here are two loops connected by two
// intersections:
// 
// /-----\
// |     |
// |  /--+--\
// |  |  |  |
// \--+--/  |
// |     |
// \-----/
// 
// Several carts are also on the tracks. Carts always face either up (^),
// down (v), left (<), or right (>). (On your initial map, the track
// under each cart is a straight path matching the direction the cart is
// facing.)
// 
// Each time a cart has the option to turn (by arriving at any
// intersection), it turns left the first time, goes straight the second
// time, turns right the third time, and then repeats those directions
// starting again with left the fourth time, straight the fifth time, and
// so on. This process is independent of the particular intersection at
// which the cart has arrived - that is, the cart has no per-intersection
// memory.
// 
// Carts all move at the same speed; they take turns moving a single step
// at a time. They do this based on their current location: carts on the
// top row move first (acting from left to right), then carts on the
// second row move (again from left to right), then carts on the third
// row, and so on. Once each cart has moved one step, the process
// repeats; each of these loops is called a tick.
// 
// For example, suppose there are two carts on a straight track:
// 
// |  |  |  |  |
// v  |  |  |  |
// |  v  v  |  |
// |  |  |  v  X
// |  |  ^  ^  |
// ^  ^  |  |  |
// |  |  |  |  |
// 
// First, the top cart moves. It is facing down (v), so it moves down one
// square. Second, the bottom cart moves. It is facing up (^), so it
// moves up one square. Because all carts have moved, the first tick
// ends. Then, the process repeats, starting with the first cart. The
// first cart moves down, then the second cart moves up - right into the
// first cart, colliding with it! (The location of the crash is marked
// with an X.) This ends the second and last tick.
// 
// Here is a longer example:
// 
// /->-\        
// |   |  /----\
// | /-+--+-\  |
// | | |  | v  |
// \-+-/  \-+--/
// \------/   
// 
// /-->\        
// |   |  /----\
// | /-+--+-\  |
// | | |  | |  |
// \-+-/  \->--/
// \------/   
// 
// /---v        
// |   |  /----\
// | /-+--+-\  |
// | | |  | |  |
// \-+-/  \-+>-/
// \------/   
// 
// /---\        
// |   v  /----\
// | /-+--+-\  |
// | | |  | |  |
// \-+-/  \-+->/
// \------/   
// 
// /---\        
// |   |  /----\
// | /->--+-\  |
// | | |  | |  |
// \-+-/  \-+--^
// \------/   
// 
// /---\        
// |   |  /----\
// | /-+>-+-\  |
// | | |  | |  ^
// \-+-/  \-+--/
// \------/   
// 
// /---\        
// |   |  /----\
// | /-+->+-\  ^
// | | |  | |  |
// \-+-/  \-+--/
// \------/   
// 
// /---\        
// |   |  /----<
// | /-+-->-\  |
// | | |  | |  |
// \-+-/  \-+--/
// \------/   
// 
// /---\        
// |   |  /---<\
// | /-+--+>\  |
// | | |  | |  |
// \-+-/  \-+--/
// \------/   
// 
// /---\        
// |   |  /--<-\
// | /-+--+-v  |
// | | |  | |  |
// \-+-/  \-+--/
// \------/   
// 
// /---\        
// |   |  /-<--\
// | /-+--+-\  |
// | | |  | v  |
// \-+-/  \-+--/
// \------/   
// 
// /---\        
// |   |  /<---\
// | /-+--+-\  |
// | | |  | |  |
// \-+-/  \-<--/
// \------/   
// 
// /---\        
// |   |  v----\
// | /-+--+-\  |
// | | |  | |  |
// \-+-/  \<+--/
// \------/   
// 
// /---\        
// |   |  /----\
// | /-+--v-\  |
// | | |  | |  |
// \-+-/  ^-+--/
// \------/   
// 
// /---\        
// |   |  /----\
// | /-+--+-\  |
// | | |  X |  |
// \-+-/  \-+--/
// \------/   
// 
// After following their respective paths for a while, the carts
// eventually crash. To help prevent crashes, you'd like to know the
// location of the first crash. Locations are given in X,Y coordinates,
// where the furthest left column is X=0 and the furthest top row is Y=0:
// 
// 111
// 0123456789012
// 0/---\        
// 1|   |  /----\
// 2| /-+--+-\  |
// 3| | |  X |  |
// 4\-+-/  \-+--/
// 5  \------/   
// 
// In this example, the location of the first crash is 7,3.
//
// --- Part Two ---
// ----------------
// 
// There isn't much you can do to prevent crashes in this ridiculous
// system. However, by predicting the crashes, the Elves know where to be
// in advance and instantly remove the two crashing carts the moment any
// crash occurs.
// 
// They can proceed like this for a while, but eventually, they're going
// to run out of carts. It could be useful to figure out where the last
// cart that hasn't crashed will end up.
// 
// For example:
// 
// />-<\  
// |   |  
// | /<+-\
// | | | v
// \>+</ |
// |   ^
// \<->/
// 
// /---\  
// |   |  
// | v-+-\
// | | | |
// \-+-/ |
// |   |
// ^---^
// 
// /---\  
// |   |  
// | /-+-\
// | v | |
// \-+-/ |
// ^   ^
// \---/
// 
// /---\  
// |   |  
// | /-+-\
// | | | |
// \-+-/    ^     |   |
// \---/
// 
// After four very expensive crashes, a tick ends with only one cart
// remaining; its final location is 6,4.
// 
// What is the location of the last cart at the end of the first tick
// where it is the only cart left?


#include "advent_of_code/2018/day13/day13.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "absl/log/log.h"
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
    return tmp.AsString();
  }

  absl::StatusOr<absl::optional<Point>> RunStep() {
    absl::optional<Point> ret;
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
    absl::Span<absl::string_view> input) const {
  if (input.empty()) return Error("bad input");
  absl::StatusOr<CharBoard> b = ParseAsBoard(input);
  if (!b.ok()) return b.status();

  absl::StatusOr<State> state = FindKarts(*b);
  if (!state.ok()) return state.status();

  while (true) {
    VLOG(1) << "State:\n" << state->DebugString();
    absl::StatusOr<absl::optional<Point>> collision = state->RunStep();
    if (!collision.ok()) return collision.status();
    if (*collision) return PointReturn(*collision);
  }

  return Error("Left infinite loop");
}

absl::StatusOr<std::string> Day_2018_13::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.empty()) return Error("bad input");
  absl::StatusOr<CharBoard> b = ParseAsBoard(input);
  if (!b.ok()) return b.status();

  absl::StatusOr<State> state = FindKarts(*b);
  if (!state.ok()) return state.status();

  while (state->carts.size() > 1) {
    VLOG(1) << "State:\n" << state->DebugString();
    absl::StatusOr<absl::optional<Point>> collision = state->RunStep();
    if (!collision.ok()) return collision.status();
  }

  return PointReturn(state->carts[0].pos);
}

}  // namespace advent_of_code
