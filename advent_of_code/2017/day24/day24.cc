// https://adventofcode.com/2017/day/24
//
// --- Day 24: Electromagnetic Moat ---
// ------------------------------------
//
// The CPU itself is a large, black building surrounded by a bottomless
// pit. Enormous metal tubes extend outward from the side of the building
// at regular intervals and descend down into the void. There's no way to
// cross, but you need to get inside.
//
// No way, of course, other than building a bridge out of the magnetic
// components strewn about nearby.
//
// Each component has two ports, one on each end. The ports come in all
// different types, and only matching types can be connected. You take an
// inventory of the components by their port types (your puzzle input).
// Each port is identified by the number of pins it uses; more pins mean
// a stronger connection for your bridge. A 3/7 component, for example,
// has a type-3 port on one side, and a type-7 port on the other.
//
// Your side of the pit is metallic; a perfect surface to connect a
// magnetic, zero-pin port. Because of this, the first port you use must
// be of type 0. It doesn't matter what type of port you end with; your
// goal is just to make the bridge as strong as possible.
//
// The strength of a bridge is the sum of the port types in each
// component. For example, if your bridge is made of components 0/3, 3/7,
// and 7/4, your bridge has a strength of 0+3 + 3+7 + 7+4 = 24.
//
// For example, suppose you had the following components:
//
// 0/2
// 2/2
// 2/3
// 3/4
// 3/5
// 0/1
// 10/1
// 9/10
//
// With them, you could make the following valid bridges:
//
// * 0/1
//
// * 0/1--10/1
//
// * 0/1--10/1--9/10
//
// * 0/2
//
// * 0/2--2/3
//
// * 0/2--2/3--3/4
//
// * 0/2--2/3--3/5
//
// * 0/2--2/2
//
// * 0/2--2/2--2/3
//
// * 0/2--2/2--2/3--3/4
//
// * 0/2--2/2--2/3--3/5
//
// (Note how, as shown by 10/1, order of ports within a component doesn't
// matter. However, you may only use each port on a component once.)
//
// Of these bridges, the strongest one is 0/1--10/1--9/10; it has a
// strength of 0+1 + 1+10 + 10+9 = 31.
//
// What is the strength of the strongest bridge you can make with the
// components you have available?
//
// --- Part Two ---
// ----------------
//
// The bridge you've built isn't long enough; you can't jump the rest of
// the way.
//
// In the example above, there are two longest bridges:
//
// * 0/2--2/2--2/3--3/4
//
// * 0/2--2/2--2/3--3/5
//
// Of them, the one which uses the 3/5 component is stronger; its
// strength is 0+2 + 2+2 + 2+3 + 3+5 = 19.
//
// What is the strength of the longest bridge you can make? If you can
// make multiple bridges of the longest length, pick the strongest one.

#include "advent_of_code/2017/day24/day24.h"

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
