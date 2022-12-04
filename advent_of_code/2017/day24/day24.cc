// http://adventofcode.com/2017/day/24

#include "advent_of_code/2017/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Converter {
  int in;
  int out;
  bool operator<(const Converter& o) const {
    if (in != o.in) return in < o.in;
    return out < o.out;
  }
};

using Memo = absl::flat_hash_map<std::pair<int, std::set<int>>, int>;

int StrongestBridge(Memo* memo, const std::vector<Converter>& ordered,
                    int ports, std::set<int> used) {
  auto memo_key = std::make_pair(ports, used);
  if (auto it = memo->find(memo_key); it != memo->end()) {
    return it->second;
  }

  int strongest_bridge = 0;
  // TODO(@monkeynova): We should have an index on the ports to avoid a full
  // linear scan.
  for (int i = 0; i < ordered.size(); ++i) {
    if (auto it = used.find(i); it != used.end()) continue;
    int next_strongest = -1;
    if (ordered[i].in == ports) {
      used.insert(i);
      next_strongest = StrongestBridge(memo, ordered, ordered[i].out, used);
      used.erase(i);
    }
    if (ordered[i].out == ports) {
      used.insert(i);
      next_strongest = StrongestBridge(memo, ordered, ordered[i].in, used);
      used.erase(i);
    }
    if (next_strongest != -1) {
      int this_bridge = next_strongest + ordered[i].in + ordered[i].out;
      if (this_bridge > strongest_bridge) {
        strongest_bridge = this_bridge;
      }
    }
  }

  VLOG(1) << "StrongestBridge(" << ports << ", " << absl::StrJoin(used, ",")
          << ") = " << strongest_bridge;

  (*memo)[memo_key] = strongest_bridge;
  return strongest_bridge;
}

int StrongestBridge(const std::vector<Converter>& ordered) {
  Memo memo;
  return StrongestBridge(&memo, ordered, 0, std::set<int>{});
}

struct LengthStrength {
  int length;
  int strength;
  bool operator>(const LengthStrength& o) const {
    if (length != o.length) return length > o.length;
    return strength > o.strength;
  }
};

using Memo2 =
    absl::flat_hash_map<std::pair<int, std::set<int>>, LengthStrength>;

LengthStrength StrongestLongestBridge(Memo2* memo,
                                      const std::vector<Converter>& ordered,
                                      int ports, std::set<int> used) {
  auto memo_key = std::make_pair(ports, used);
  if (auto it = memo->find(memo_key); it != memo->end()) {
    return it->second;
  }

  LengthStrength ret = {0, 0};
  // TODO(@monkeynova): We should have an index on the ports to avoid a full
  // linear scan.
  for (int i = 0; i < ordered.size(); ++i) {
    if (auto it = used.find(i); it != used.end()) continue;
    LengthStrength next = {-1, -1};
    if (ordered[i].in == ports) {
      used.insert(i);
      next = StrongestLongestBridge(memo, ordered, ordered[i].out, used);
      used.erase(i);
    }
    if (ordered[i].out == ports) {
      used.insert(i);
      next = StrongestLongestBridge(memo, ordered, ordered[i].in, used);
      used.erase(i);
    }
    if (next.length == -1) continue;
    ++next.length;
    next.strength += ordered[i].in + ordered[i].out;
    if (next > ret) {
      ret = next;
    }
  }

  VLOG(1) << "StrongestBridge(" << ports << ", " << absl::StrJoin(used, ",")
          << ") = " << ret.length << ", " << ret.strength;

  (*memo)[memo_key] = ret;
  return ret;
}

LengthStrength StrongestLongestBridge(const std::vector<Converter>& ordered) {
  Memo2 memo;
  return StrongestLongestBridge(&memo, ordered, 0, std::set<int>{});
}

}  // namespace

absl::StatusOr<std::string> Day_2017_24::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<Converter> convs;
  for (absl::string_view row : input) {
    Converter c;
    if (!RE2::FullMatch(row, "(\\d+)/(\\d+)", &c.in, &c.out)) {
      return Error("Bad line: ", row);
    }
    convs.push_back(c);
  }
  return IntReturn(StrongestBridge(convs));
}

absl::StatusOr<std::string> Day_2017_24::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<Converter> convs;
  for (absl::string_view row : input) {
    Converter c;
    if (!RE2::FullMatch(row, "(\\d+)/(\\d+)", &c.in, &c.out)) {
      return Error("Bad line: ", row);
    }
    convs.push_back(c);
  }
  return IntReturn(StrongestLongestBridge(convs).strength);
}

}  // namespace advent_of_code
