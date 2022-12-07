// https://adventofcode.com/2016/day/7
//
// --- Day 7: Internet Protocol Version 7 ---
// ------------------------------------------
// 
// While snooping around the local network of EBHQ, you compile a list of
// IP addresses (they're IPv7, of course; IPv6 is much too limited).
// You'd like to figure out which IPs support TLS (transport-layer
// snooping).
// 
// An IP supports TLS if it has an Autonomous Bridge Bypass Annotation,
// or ABBA. An ABBA is any four-character sequence which consists of a
// pair of two different characters followed by the reverse of that pair,
// such as xyyx or abba. However, the IP also must not have an ABBA
// within any hypernet sequences, which are contained by square brackets.
// 
// For example:
// 
// * abba[mnop]qrst supports TLS (abba outside square brackets).
// 
// * abcd[bddb]xyyx does not support TLS (bddb is within square
// brackets, even though xyyx is outside square brackets).
// 
// * aaaa[qwer]tyui does not support TLS (aaaa is invalid; the interior
// characters must be different).
// 
// * ioxxoj[asdfgh]zxcvbn supports TLS (oxxo is outside square
// brackets, even though it's within a larger string).
// 
// How many IPs in your puzzle input support TLS?
//
// --- Part Two ---
// ----------------
// 
// You would also like to know which IPs support SSL (super-secret
// listening).
// 
// An IP supports SSL if it has an Area-Broadcast Accessor, or ABA,
// anywhere in the supernet sequences (outside any square bracketed
// sections), and a corresponding Byte Allocation Block, or BAB, anywhere
// in the hypernet sequences. An ABA is any three-character sequence
// which consists of the same character twice with a different character
// between them, such as xyx or aba. A corresponding BAB is the same
// characters but in reversed positions: yxy and bab, respectively.
// 
// For example:
// 
// * aba[bab]xyz supports SSL (aba outside square brackets with
// corresponding bab within square brackets).
// 
// * xyx[xyx]xyx does not support SSL (xyx, but no corresponding yxy).
// 
// * aaa[kek]eke supports SSL (eke in supernet with corresponding kek
// in hypernet; the aaa sequence is not related, because the interior
// character must be different).
// 
// * zazbz[bzb]cdb supports SSL (zaz has no corresponding aza, but zbz
// has a corresponding bzb, even though zaz and zbz overlap).
// 
// How many IPs in your puzzle input support SSL?


#include "advent_of_code/2016/day07/day07.h"

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

bool SupportsTLS(absl::string_view str) {
  int level = 0;
  bool match_outside = false;
  bool match_inside = false;
  for (int i = 0; i < str.size(); ++i) {
    if (str[i] == '[') ++level;
    if (str[i] == ']') --level;
    if (i >= 3) {
      if (str[i] == str[i - 3] && str[i - 1] == str[i - 2] &&
          str[i] != str[i - 1]) {
        if (level == 0)
          match_outside = true;
        else
          match_inside = true;
      }
    }
  }
  return match_outside && !match_inside;
}

bool SupportsSSL(absl::string_view str) {
  int level = 0;
  absl::flat_hash_set<std::string> outside;
  absl::flat_hash_set<std::string> inside;
  for (int i = 0; i < str.size(); ++i) {
    if (str[i] == '[') ++level;
    if (str[i] == ']') --level;
    if (i >= 2) {
      if (str[i] == str[i - 2] && str[i] != str[i - 1]) {
        if (level == 0) {
          // ABA
          char tmp[] = {str[i], str[i - 1], '\0'};
          outside.insert(std::string(tmp));
        } else {
          // BAB
          char tmp[] = {str[i - 1], str[i], '\0'};
          inside.insert(std::string(tmp));
        }
      }
    }
  }
  VLOG(1) << str << ": " << absl::StrJoin(outside, ",") << "; "
          << absl::StrJoin(inside, ",");
  for (const auto& s : outside) {
    if (inside.contains(s)) return true;
  }
  return false;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2016_07::Part1(
    absl::Span<absl::string_view> input) const {
  int count = 0;
  for (absl::string_view str : input) {
    bool supports = SupportsTLS(str);
    VLOG(1) << str << ": " << supports;
    if (supports) ++count;
  }
  return IntReturn(count);
}

absl::StatusOr<std::string> Day_2016_07::Part2(
    absl::Span<absl::string_view> input) const {
  int count = 0;
  for (absl::string_view str : input) {
    bool supports = SupportsSSL(str);
    VLOG(1) << str << ": " << supports;
    if (supports) ++count;
  }
  return IntReturn(count);
}

}  // namespace advent_of_code
