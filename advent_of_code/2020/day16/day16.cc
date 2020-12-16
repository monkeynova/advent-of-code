#include "advent_of_code/2020/day16/day16.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

struct OrRange {
  int min1;
  int max1;
  int min2;
  int max2;
};

struct ParseResult {
  absl::flat_hash_map<absl::string_view, OrRange> rules;
  std::vector<int64_t> my_ticket;
  std::vector<std::vector<int64_t>> other_tickets;
};

absl::StatusOr<ParseResult> Parse(absl::Span<absl::string_view> input) {
  ParseResult res;
  int i = 0;
  for (; i < input.size(); ++i) {
    absl::string_view col;
    OrRange or_range;
    if (!RE2::FullMatch(input[i], "(.*): (\\d+)-(\\d+) or (\\d+)-(\\d+)",
                        &col, &or_range.min1, &or_range.max1, &or_range.min2, &or_range.max2)) break;
    if (res.rules.contains(col)) return AdventDay::Error("Duplicate rule: ", col);
    res.rules.emplace(col, or_range);
  }
  if (!input[i].empty()) return AdventDay::Error("Not empty line: ", input[i]);
  ++i;
  if (input[i] != "your ticket:") return AdventDay::Error("Not 'your ticket' ", input[i]);
  ++i;
  std::vector<absl::string_view> my_ticket_str = absl::StrSplit(input[i], ",");
  absl::StatusOr<std::vector<int64_t>> my_ticket = AdventDay::ParseAsInts(absl::MakeSpan(my_ticket_str));
  if (!my_ticket.ok()) return my_ticket.status();
  res.my_ticket = *my_ticket;
  ++i;

  if (!input[i].empty()) return AdventDay::Error("Not empty line: ", input[i]);
  ++i;
  if (input[i] != "nearby tickets:") return AdventDay::Error("Not 'nearby tickets' ", input[i]);
  ++i;
  for (; i < input.size(); ++i) {
    std::vector<absl::string_view> other_ticket_str = absl::StrSplit(input[i], ",");
    absl::StatusOr<std::vector<int64_t>> other_ticket = AdventDay::ParseAsInts(absl::MakeSpan(other_ticket_str));
    if (!other_ticket.ok()) return other_ticket.status();
    res.other_tickets.push_back(*other_ticket);
  }

  return res;
}

absl::StatusOr<std::vector<std::string>> Day16_2020::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<ParseResult> parse = Parse(input);
  if (!parse.ok()) return parse.status();
  int bad_val_sum = 0;
  for (const std::vector<int64_t>& ticket : parse->other_tickets) {
    for (int v : ticket) {
      VLOG(1) << "Checking: " << v;
      bool valid_any = false;
      for (const auto& [name, or_range] : parse->rules) {
        if (v >= or_range.min1 && v <= or_range.max1) {
          VLOG(2) << "Valid: [" << or_range.min1 << "," <<  or_range.max1 << "],["
                  <<or_range.min2 << "," <<  or_range.max2 << "]";
          valid_any = true;
          break;
        }
        if (v >= or_range.min2 && v <= or_range.max2) {
          VLOG(2) << "Valid: [" << or_range.min1 << "," <<  or_range.max1 << "],["
                  <<or_range.min2 << "," <<  or_range.max2 << "]";
          valid_any = true;
          break;
        }
      }
      if (!valid_any) {
        VLOG(1) << "Invalid";
        bad_val_sum += v;
      }
    }
  }
  return IntReturn(bad_val_sum);
}

absl::StatusOr<std::vector<std::string>> Day16_2020::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}
