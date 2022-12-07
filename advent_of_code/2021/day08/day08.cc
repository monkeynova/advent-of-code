// https://adventofcode.com/2021/day/8
//
// --- Day 8: Seven Segment Search ---
// -----------------------------------
//
// You barely reach the safety of the cave when the whale smashes into
// the cave mouth, collapsing it. Sensors indicate another exit to this
// cave at a much greater depth, so you have no choice but to press on.
//
// As your submarine slowly makes its way through the cave system, you
// notice that the four-digit seven-segment displays in your submarine
// are malfunctioning; they must have been damaged during the escape.
// You'll be in a lot of trouble without them, so you'd better figure out
// what's wrong.
//
// Each digit of a seven-segment display is rendered by turning on or off
// any of seven segments named a through g:
//
//   0:      1:      2:      3:      4:
//  aaaa    ....    aaaa    aaaa    ....
// b    c  .    c  .    c  .    c  b    c
// b    c  .    c  .    c  .    c  b    c
//  ....    ....    dddd    dddd    dddd
// e    f  .    f  e    .  .    f  .    f
// e    f  .    f  e    .  .    f  .    f
//  gggg    ....    gggg    gggg    ....
// 
//   5:      6:      7:      8:      9:
//  aaaa    aaaa    aaaa    aaaa    aaaa
// b    .  b    .  .    c  b    c  b    c
// b    .  b    .  .    c  b    c  b    c
//  dddd    dddd    ....    dddd    dddd
// .    f  e    f  .    f  e    f  .    f
// .    f  e    f  .    f  e    f  .    f
//  gggg    gggg    ....    gggg    gggg// 
// So, to render a 1, only segments c and f would be turned on; the rest
// would be off. To render a 7, only segments a, c, and f would be turned
// on.
//
// The problem is that the signals which control the segments have been
// mixed up on each display. The submarine is still trying to display
// numbers by producing output on signal wires a through g, but those
// wires are connected to segments randomly. Worse, the wire/segment
// connections are mixed up separately for each four-digit display! (All
// of the digits within a display use the same connections, though.)
//
// So, you might know that only signal wires b and g are turned on, but
// that doesn't mean segments b and g are turned on: the only digit that
// uses two segments is 1, so it must mean segments c and f are meant to
// be on. With just that information, you still can't tell which wire (b/g)
// goes to which segment (c/f). For that, you'll need to collect more
// information.
//
// For each display, you watch the changing signals for a while, make a
// note of all ten unique signal patterns you see, and then write down a
// single four digit output value (your puzzle input). Using the signal
// patterns, you should be able to work out which pattern corresponds to
// which digit.
//
// For example, here is what you might see in a single entry in your
// notes:
//
// acedgfb cdfbe gcdfa fbcad dab cefabd cdfgeb eafb cagedb ab |
// cdfeb fcadb cdfeb cdbaf
//
// (The entry is wrapped here to two lines so it fits; in your notes, it
// will all be on a single line.)
//
// Each entry consists of ten unique signal patterns, a | delimiter, and
// finally the four digit output value. Within an entry, the same
// wire/segment connections are used (but you don't know what the
// connections actually are). The unique signal patterns correspond to
// the ten different ways the submarine tries to render a digit using the
// current wire/segment connections. Because 7 is the only digit that
// uses three segments, dab in the above example means that to render a 7,
// signal lines d, a, and b are on. Because 4 is the only digit that uses
// four segments, eafb means that to render a 4, signal lines e, a, f,
// and b are on.
//
// Using this information, you should be able to work out which
// combination of signal wires corresponds to each of the ten digits.
// Then, you can decode the four digit output value. Unfortunately, in
// the above example, all of the digits in the output value (cdfeb fcadb
// cdfeb cdbaf) use five segments and are more difficult to deduce.
//
// For now, focus on the easy digits. Consider this larger example:
//
// be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb |   fdgacbe cefdb
// cefbgd    gcbe   edbfga begcd cbg gc gcadebf fbgde acbgfd abcde gfcbed gfec |
// fcgedb    cgb       dgebacf       gc   fgaebd cg bdaec gdafb agbcfd gdcbef
// bgcad gfac gcb cdgabef |   cg       cg    fdcagb    cbg   fbegcd cbd adcefb
// dageb afcb bc aefdc ecdab fgdeca fcdbega | efabcd cedba gadfec    cb aecbfdg
// fbg gf bafeg dbefa fcge gcbea fcaegb dgceab fcbdga |   gecf       egdcabf bgf
// bfgea fgeab ca afcebg bdacfeg cfaedg gcfdb baec bfadeg bafgc acf |   gebdcfa
// ecba       ca       fadegcb   dbcfg fgd bdegcaf fgec aegbdf ecdfab fbedc
// dacgb gdcebf gf |   cefg    dcbef    fcge       gbcadfe   bdfegc cbegaf gecbf
// dfcage bdacg ed bedf ced adcbefg gebcd |   ed    bcgafe cdgba cbgef egadfb
// cdbfeg cegd fecab cgb gbdefca cg fgcdab egfdb bfceg |   gbdfcae       bgc cg
// cgb   gcafb gcf dcaebfg ecagb gf abcdeg gaef cafbge fdbac fegbdc |   fgae
// cfgab    fg    bagce
//
// Because the digits 1, 4, 7, and 8 each use a unique number of
// segments, you should be able to tell which combinations of signals
// correspond to those digits. Counting only digits in the output values
// (the part after | on each line), in the above example, there are 26
// instances of digits that use a unique number of segments (highlighted
// above).
//
// In the output values, how many times do digits 1, 4, 7, or 8 appear?
//
// --- Part Two ---
// ----------------
//
// Through a little deduction, you should now be able to determine the
// remaining digits. Consider again the first example above:
//
// acedgfb cdfbe gcdfa fbcad dab cefabd cdfgeb eafb cagedb ab |
// cdfeb fcadb cdfeb cdbaf
//
// After some careful analysis, the mapping between signal wires and
// segments only make sense in the following configuration:
//
// dddd
// e    a
// e    a
// ffff
// g    b
// g    b
// cccc
//
// So, the unique signal patterns would correspond to the following
// digits:
//
// * acedgfb: 8
//
// * cdfbe: 5
//
// * gcdfa: 2
//
// * fbcad: 3
//
// * dab: 7
//
// * cefabd: 9
//
// * cdfgeb: 6
//
// * eafb: 4
//
// * cagedb: 0
//
// * ab: 1
//
// Then, the four digits of the output value can be decoded:
//
// * cdfeb: 5
//
// * fcadb: 3
//
// * cdfeb: 5
//
// * cdbaf: 3
//
// Therefore, the output value for this entry is 5353.
//
// Following this same process for each entry in the second, larger
// example above, the output value of each entry can be determined:
//
// * fdgacbe cefdb cefbgd gcbe: 8394
//
// * fcgedb cgb dgebacf gc: 9781
//
// * cg cg fdcagb cbg: 1197
//
// * efabcd cedba gadfec cb: 9361
//
// * gecf egdcabf bgf bfgea: 4873
//
// * gebdcfa ecba ca fadegcb: 8418
//
// * cefg dcbef fcge gbcadfe: 4548
//
// * ed bcgafe cdgba cbgef: 1625
//
// * gbdfcae bgc cg cgb: 8717
//
// * fgae cfgab fg bagce: 4315
//
// Adding all of the output values in this larger example produces 61229.
//
// For each entry, determine all of the wire/segment connections and
// decode the four-digit output values. What do you get if you add up all
// of the output values?

#include "advent_of_code/2021/day08/day08.h"

#include "absl/algorithm/container.h"
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

std::string DebugSet(const absl::flat_hash_set<char>& possible) {
  std::string ret;
  for (char c : possible) ret.append(1, c);
  return ret;
}

absl::flat_hash_set<char> Restrict(const absl::flat_hash_set<char>& possible,
                                   absl::string_view set) {
  absl::flat_hash_set<char> new_possible;
  for (char c : set) {
    if (possible.contains(c)) new_possible.insert(c);
  }
  return new_possible;
}

static const absl::flat_hash_map<std::string, int64_t> kLookup = {
    {"abcefg", 0}, {"cf", 1},     {"acdeg", 2}, {"acdfg", 3},   {"bcdf", 4},
    {"abdfg", 5},  {"abdefg", 6}, {"acf", 7},   {"abcdefg", 8}, {"abcdfg", 9},
};

absl::StatusOr<int64_t> Decode(absl::string_view digit,
                               const absl::flat_hash_map<char, char>& map) {
  std::string digit_copy(digit);
  for (char& c : digit_copy) {
    auto it = map.find(c);
    if (it == map.end()) {
      return Error("Could not find digit: ", digit_copy);
    }
    c = it->second;
  }
  absl::c_sort(digit_copy);
  auto it = kLookup.find(digit_copy);
  if (it == kLookup.end()) {
    return Error("Could not lookup digit_copy: ", digit_copy);
  }
  return it->second;
}

bool TryMap(absl::flat_hash_map<char, char>& map, char next,
            const std::vector<absl::string_view>& digits) {
  if (next == 'h') {
    for (absl::string_view d : digits) {
      absl::StatusOr<int64_t> v = Decode(d, map);
      if (!v.ok()) return false;
    }
    return true;
  }

  for (char c : {'a', 'b', 'c', 'd', 'e', 'f', 'g'}) {
    if (map.contains(c)) continue;
    map[c] = next;
    if (TryMap(map, next + 1, digits)) return true;
    map.erase(c);
  }
  return false;
}

absl::StatusOr<absl::flat_hash_map<char, char>> FindMap(
    const std::vector<absl::string_view>& left) {
  if (left.size() != 10) return Error("Bad exemplars");

  absl::flat_hash_map<int64_t, absl::flat_hash_set<char>> countToSegments = {
      {8, {'a', 'c'}}, {6, {'b'}}, {7, {'d', 'g'}}, {4, {'e'}}, {9, {'f'}},
  };

  absl::flat_hash_map<char, int64_t> observed;
  for (absl::string_view in : left) {
    for (char c : in) {
      ++observed[c];
    }
  }

  absl::flat_hash_map<char, absl::flat_hash_set<char>> possible;

  for (char c1 : {'a', 'b', 'c', 'd', 'e', 'f', 'g'}) {
    auto it = observed.find(c1);
    if (it == observed.end()) return Error("Bad observed");
    auto it2 = countToSegments.find(it->second);
    if (it2 == countToSegments.end()) return Error("Bad count");
    possible[c1] = it2->second;
  }
  for (absl::string_view test : left) {
    absl::string_view allowed = "";
    switch (test.size()) {
      case 2:
        allowed = "cf";
        break;
      case 3:
        allowed = "acf";
        break;
      case 4:
        allowed = "bcdf";
        break;
    }
    if (allowed.empty()) continue;
    for (char c : test) {
      possible[c] = Restrict(possible[c], allowed);
    }
  }
  absl::flat_hash_set<char> definitive;
  for (const auto& [c1, set] : possible) {
    if (set.size() == 1) definitive.insert(*set.begin());
  }
  bool more_work = true;
  while (more_work) {
    more_work = false;
    for (auto& [c1, set] : possible) {
      if (set.size() > 1) {
        absl::flat_hash_set<char> new_set;
        for (char c : set) {
          if (!definitive.contains(c)) new_set.insert(c);
        }
        set = std::move(new_set);
        if (set.size() > 1) {
          more_work = true;
        }
      }
    }
  }
  absl::flat_hash_map<char, char> ret;
  for (const auto& [c, set] : possible) {
    if (set.size() != 1) return Error("Bad possible");
    ret[c] = *set.begin();
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2021_08::Part1(
    absl::Span<absl::string_view> input) const {
  int64_t count = 0;
  absl::flat_hash_set<int64_t> find_sizes = {2, 3, 4, 7};
  for (absl::string_view line : input) {
    const auto& [_, find] = PairSplit(line, " | ");
    for (absl::string_view segment : absl::StrSplit(find, " ")) {
      if (find_sizes.contains(segment.size())) ++count;
    }
  }
  return IntReturn(count);
}

absl::StatusOr<std::string> Day_2021_08::Part2(
    absl::Span<absl::string_view> input) const {
  int64_t sum = 0;
  for (absl::string_view line : input) {
    const auto& [exemplars, decode] = PairSplit(line, " | ");
    absl::StatusOr<absl::flat_hash_map<char, char>> map =
        FindMap(absl::StrSplit(exemplars, " "));
    if (!map.ok()) {
      return Error("Could not make map ", map.status().ToString(), " for ",
                   exemplars);
    }
    int64_t decode_val = 0;
    for (absl::string_view digit : absl::StrSplit(decode, " ")) {
      absl::StatusOr<int64_t> v = Decode(digit, *map);
      if (!v.ok()) return v.status();
      decode_val = decode_val * 10 + *v;
    }
    sum += decode_val;
  }
  return IntReturn(sum);
}

}  // namespace advent_of_code
