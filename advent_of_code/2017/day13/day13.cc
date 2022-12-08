#include "advent_of_code/2017/day13/day13.h"

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

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2017_13::Part1(
    absl::Span<absl::string_view> input) const {
  int severity = 0;
  for (absl::string_view row : input) {
    int depth;
    int range;
    if (!RE2::FullMatch(row, "(\\d+): (\\d+)", &depth, &range)) {
      return Error("Bad row: ", row);
    }
    int pos = depth % (2 * (range - 1));
    VLOG(2) << "depth=" << depth << "; range=" << range << "; pos=" << pos;
    if (pos == 0) {
      severity += depth * range;
    }
  }
  return IntReturn(severity);
}

absl::StatusOr<std::string> Day_2017_13::Part2(
    absl::Span<absl::string_view> input) const {
  struct Firewall {
    int depth;
    int range;
  };
  std::vector<Firewall> firewalls;
  for (absl::string_view row : input) {
    Firewall fw;
    if (!RE2::FullMatch(row, "(\\d+): (\\d+)", &fw.depth, &fw.range)) {
      return Error("Bad row: ", row);
    }
    firewalls.push_back(fw);
  }

  int delay = -1;
  bool caught = true;
  while (caught) {
    ++delay;
    VLOG(1) << "delay=" << delay;
    caught = false;
    for (Firewall fw : firewalls) {
      int pos = (delay + fw.depth) % (2 * (fw.range - 1));
      VLOG(2) << "depth=" << fw.depth << "; range=" << fw.range
              << "; pos=" << pos;
      if (pos == 0) {
        VLOG(1) << "Caught @" << delay;
        caught = true;
        break;
      }
    }
  }
  return IntReturn(delay);
}

}  // namespace advent_of_code
