#include "advent_of_code/2017/day16/day16.h"

#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/loop_history.h"
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

absl::StatusOr<std::vector<DanceMove>> Parse(std::string_view in) {
  std::vector<std::string_view> ins = absl::StrSplit(in, ",");
  std::vector<DanceMove> ret;
  for (std::string_view i : ins) {
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
        std::string_view line_view = line->vals;
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

}  // namespace

absl::StatusOr<std::string> Day_2017_16::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  ASSIGN_OR_RETURN(std::vector<DanceMove> moves, Parse(input[0]));
  Line line;
  RunDance(moves, &line);
  return line.vals;
}

absl::StatusOr<std::string> Day_2017_16::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  ASSIGN_OR_RETURN(std::vector<DanceMove> moves, Parse(input[0]));
  Line line;
  LoopHistory<std::string> hist;
  for (int i = 0; i < 1'000'000'000; ++i) {
    if (hist.AddMaybeNew(line.vals)) {
      std::string_view vals = hist.FindInLoop(1'000'000'000);
      return AdventReturn(vals);
    }
    VLOG_IF(1, i % 77'777 == 0) << i;
    RunDance(moves, &line);
  }
  return line.vals;
}

}  // namespace advent_of_code
