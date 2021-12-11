#include "advent_of_code/2017/day16/day16.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
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
