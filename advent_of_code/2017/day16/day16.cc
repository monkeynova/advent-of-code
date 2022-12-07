// https://adventofcode.com/2017/day/16
//
// --- Day 16: Permutation Promenade ---
// -------------------------------------
//
// You come upon a very unusual sight; a group of programs here appear to
// be dancing.
//
// There are sixteen programs in total, named a through p. They start by
// standing in a line: a stands in position 0, b stands in position 1,
// and so on until p, which stands in position 15.
//
// The programs' dance consists of a sequence of dance moves:
//
// * Spin, written sX, makes X programs move from the end to the front,
// but maintain their order otherwise. (For example, s3 on abcde
// produces cdeab).
//
// * Exchange, written xA/B, makes the programs at positions A and B
// swap places.
//
// * Partner, written pA/B, makes the programs named A and B swap
// places.
//
// For example, with only five programs standing in a line (abcde), they
// could do the following dance:
//
// * s1, a spin of size 1: eabcd.
//
// * x3/4, swapping the last two programs: eabdc.
//
// * pe/b, swapping programs e and b: baedc.
//
// After finishing their dance, the programs end up in order baedc.
//
// You watch the dance for a while and record their dance moves (your
// puzzle input). In what order are the programs standing after their
// dance?
//
// --- Part Two ---
// ----------------
//
// Now that you're starting to get a feel for the dance moves, you turn
// your attention to the dance as a whole.
//
// Keeping the positions they ended up in from their previous dance, the
// programs perform it again and again: including the first dance, a
// total of one billion (1000000000) times.
//
// In the example above, their second dance would begin with the order
// baedc, and use the same dance moves:
//
// * s1, a spin of size 1: cbaed.
//
// * x3/4, swapping the last two programs: cbade.
//
// * pe/b, swapping programs e and b: ceadb.
//
// In what order are the programs standing after their billion dances?

#include "advent_of_code/2017/day16/day16.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct DanceMove {
  enum {
    kSpin = 1,
    kExchange = 2,
    kPartner = 3,
  } op;
  int n1;
  int n2;
  char c1;
  char c2;
};

absl::StatusOr<std::vector<DanceMove>> Parse(absl::string_view in) {
  std::vector<absl::string_view> ins = absl::StrSplit(in, ",");
  std::vector<DanceMove> ret;
  for (absl::string_view i : ins) {
    DanceMove next;
    if (RE2::FullMatch(i, "s(\\d+)", &next.n1)) {
      next.op = DanceMove::kSpin;
    } else if (RE2::FullMatch(i, "x(\\d+)/(\\d+)", &next.n1, &next.n2)) {
      next.op = DanceMove::kExchange;
    } else if (RE2::FullMatch(i, "p([a-p])/([a-p])", &next.c1, &next.c2)) {
      next.op = DanceMove::kPartner;
    } else {
      return Error("Bad instruction");
    }
    ret.push_back(next);
  }
  return ret;
}

struct Line {
  std::string vals = "abcdefghijklmnop";
};

void RunDance(const std::vector<DanceMove>& moves, Line* line) {
  for (const DanceMove& m : moves) {
    switch (m.op) {
      case DanceMove::kSpin: {
        absl::string_view line_view = line->vals;
        int split = line_view.size() - m.n1;
        line->vals =
            absl::StrCat(line_view.substr(split), line_view.substr(0, split));
        break;
      }
      case DanceMove::kExchange: {
        std::swap(line->vals[m.n1], line->vals[m.n2]);
        break;
      }
      case DanceMove::kPartner: {
        int n1, n2;
        for (int i = 0; i < line->vals.length(); ++i) {
          if (m.c1 == line->vals[i]) n1 = i;
          if (m.c2 == line->vals[i]) n2 = i;
        }
        std::swap(line->vals[n1], line->vals[n2]);
        break;
      }
    }
  }
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2017_16::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  absl::StatusOr<std::vector<DanceMove>> moves = Parse(input[0]);
  if (!moves.ok()) return moves.status();
  Line line;
  RunDance(*moves, &line);
  return line.vals;
}

absl::StatusOr<std::string> Day_2017_16::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  absl::StatusOr<std::vector<DanceMove>> moves = Parse(input[0]);
  if (!moves.ok()) return moves.status();
  Line line;
  absl::flat_hash_map<std::string, int> history;
  int loop_size = -1;
  int loop_offset;
  for (int i = 0; i < 1'000'000'000; ++i) {
    auto it = history.find(line.vals);
    if (it != history.end()) {
      loop_size = i - history[line.vals];
      loop_offset = history[line.vals];
      break;
    }
    history[line.vals] = i;
    VLOG_IF(1, i % 77'777 == 0) << i;
    RunDance(*moves, &line);
  }
  if (loop_size != -1) {
    VLOG(1) << "Found loop: size=" << loop_size << "; offset=" << loop_offset;
    int need_offset = (1'000'000'000 - loop_offset) % loop_size + loop_offset;
    VLOG(1) << "  need_offset=" << need_offset;
    Line new_line;
    for (int i = 0; i < need_offset; ++i) {
      RunDance(*moves, &new_line);
    }
    line = new_line;
  }
  return line.vals;
}

}  // namespace advent_of_code
