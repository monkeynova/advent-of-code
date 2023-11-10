#include "advent_of_code/2020/day16/day16.h"

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

struct OrRange {
  int min1;
  int max1;
  int min2;
  int max2;
};

struct ParseResult {
  absl::flat_hash_map<std::string_view, OrRange> rules;
  std::vector<int64_t> my_ticket;
  std::vector<std::vector<int64_t>> other_tickets;
};

absl::StatusOr<ParseResult> Parse(absl::Span<std::string_view> input) {
  ParseResult res;
  int i = 0;
  for (; i < input.size(); ++i) {
    std::string_view col;
    OrRange or_range;
    if (!RE2::FullMatch(input[i], "(.*): (\\d+)-(\\d+) or (\\d+)-(\\d+)", &col,
                        &or_range.min1, &or_range.max1, &or_range.min2,
                        &or_range.max2))
      break;
    if (res.rules.contains(col)) return Error("Duplicate rule: ", col);
    res.rules.emplace(col, or_range);
  }
  if (!input[i].empty()) return Error("Not empty line: ", input[i]);
  ++i;
  if (input[i] != "your ticket:") return Error("Not 'your ticket' ", input[i]);
  ++i;
  std::vector<std::string_view> my_ticket_str = absl::StrSplit(input[i], ",");
  absl::StatusOr<std::vector<int64_t>> my_ticket =
      ParseAsInts(absl::MakeSpan(my_ticket_str));
  if (!my_ticket.ok()) return my_ticket.status();
  res.my_ticket = *my_ticket;
  ++i;

  if (!input[i].empty()) return Error("Not empty line: ", input[i]);
  ++i;
  if (input[i] != "nearby tickets:")
    return Error("Not 'nearby tickets' ", input[i]);
  ++i;
  for (; i < input.size(); ++i) {
    std::vector<std::string_view> other_ticket_str =
        absl::StrSplit(input[i], ",");
    absl::StatusOr<std::vector<int64_t>> other_ticket =
        ParseAsInts(absl::MakeSpan(other_ticket_str));
    if (!other_ticket.ok()) return other_ticket.status();
    res.other_tickets.push_back(*other_ticket);
  }

  return res;
}

bool CheckRange(int v, OrRange or_range) {
  if (v >= or_range.min1 && v <= or_range.max1) {
    VLOG(3) << "Valid: [" << or_range.min1 << "," << or_range.max1 << "],["
            << or_range.min2 << "," << or_range.max2 << "]";
    return true;
  }
  if (v >= or_range.min2 && v <= or_range.max2) {
    VLOG(3) << "Valid: [" << or_range.min1 << "," << or_range.max1 << "],["
            << or_range.min2 << "," << or_range.max2 << "]";
    return true;
  }
  return false;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_16::Part1(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<ParseResult> parse = Parse(input);
  if (!parse.ok()) return parse.status();
  int bad_val_sum = 0;
  for (const std::vector<int64_t>& ticket : parse->other_tickets) {
    for (int v : ticket) {
      VLOG(2) << "Checking: " << v;
      bool valid_any = false;
      for (const auto& [name, or_range] : parse->rules) {
        if (CheckRange(v, or_range)) {
          valid_any = true;
          break;
        }
      }
      if (!valid_any) {
        VLOG(2) << "Invalid";
        bad_val_sum += v;
      }
    }
  }
  return AdventReturn(bad_val_sum);
}

bool IsValid(const ParseResult& parse, std::string_view field_name, int pos) {
  auto it = parse.rules.find(field_name);
  CHECK(it != parse.rules.end());
  OrRange or_range = it->second;
  VLOG(3) << "IsValid: " << field_name << ": " << pos;
  for (const std::vector<int64_t>& ticket : parse.other_tickets) {
    if (!CheckRange(ticket[pos], or_range)) return false;
  }
  VLOG(3) << "  = true";
  return true;
}

bool FindRemainingFieldOrder(
    const ParseResult& parse,
    const absl::flat_hash_map<std::string_view, absl::flat_hash_set<int>>&
        is_valid,
    std::vector<std::string_view>* ret,
    const absl::flat_hash_set<std::string_view>& remaining) {
  VLOG(3) << "FindRemaningFieldOrder: " << absl::StrJoin(*ret, ",");
  absl::flat_hash_set<std::string_view> sub_remaining = remaining;
  for (std::string_view field : remaining) {
    if (auto it = is_valid.find(field); !it->second.contains(ret->size())) {
      continue;
    }
    ret->push_back(field);
    sub_remaining.erase(field);
    if (sub_remaining.empty()) return true;
    if (FindRemainingFieldOrder(parse, is_valid, ret, sub_remaining))
      return true;
    sub_remaining.insert(field);
    ret->pop_back();
  }
  return false;
}

void BurnIsValid(absl::flat_hash_map<std::string_view,
                                     absl::flat_hash_set<int>>* is_valid) {
  std::vector<std::string_view> singletons;
  for (const auto& [field, map] : *is_valid) {
    if (map.size() == 1) singletons.push_back(field);
  }
  while (!singletons.empty()) {
    std::vector<std::string_view> new_singletons;
    for (std::string_view singleton_field : singletons) {
      int pos = *is_valid->find(singleton_field)->second.begin();
      VLOG(1) << "removing: pos=" << pos << "; singleton=" << singleton_field;
      for (auto& [field, map] : *is_valid) {
        if (singleton_field == field) continue;
        if (!map.contains(pos)) continue;
        map.erase(pos);
        if (map.size() == 1) new_singletons.push_back(field);
      }
    }
    singletons = std::move(new_singletons);
  }
}

std::vector<std::string_view> FindFieldOrder(const ParseResult& parse) {
  std::vector<std::string_view> ret;
  absl::flat_hash_set<std::string_view> remaining;
  for (const auto& [name, _] : parse.rules) {
    remaining.insert(name);
  }
  absl::flat_hash_map<std::string_view, absl::flat_hash_set<int>> is_valid_map;
  for (std::string_view field : remaining) {
    for (int pos = 0; pos < remaining.size(); ++pos) {
      bool is_valid = IsValid(parse, field, pos);
      VLOG(2) << "IsValid(" << field << "," << pos << ") = " << is_valid;
      if (is_valid) {
        is_valid_map[field].insert(pos);
      }
    }
    VLOG(1) << "IsValid(" << field << ") = " << is_valid_map[field].size();
  }
  BurnIsValid(&is_valid_map);
  CHECK(FindRemainingFieldOrder(parse, is_valid_map, &ret, remaining));
  return ret;
}

absl::StatusOr<std::string> Day_2020_16::Part2(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<ParseResult> parse = Parse(input);
  if (!parse.ok()) return parse.status();

  std::vector<std::vector<int64_t>> valid_tickets;
  for (const std::vector<int64_t>& ticket : parse->other_tickets) {
    bool valid_ticket = true;
    for (int v : ticket) {
      VLOG(2) << "Checking: " << v;
      bool valid_any_value = false;
      for (const auto& [name, or_range] : parse->rules) {
        if (CheckRange(v, or_range)) {
          valid_any_value = true;
          break;
        }
      }
      if (!valid_any_value) {
        valid_ticket = false;
        break;
      }
    }
    if (valid_ticket) {
      valid_tickets.push_back(ticket);
    }
  }
  VLOG(1) << valid_tickets.size() << " valid tickets";
  VLOG(2) << absl::StrJoin(valid_tickets, "\n",
                           [](std::string* out, const std::vector<int64_t>& t) {
                             absl::StrAppend(out, absl::StrJoin(t, ","));
                           });
  parse->other_tickets = valid_tickets;
  std::vector<std::string_view> field_order = FindFieldOrder(*parse);
  VLOG(1) << "found size == " << field_order.size() << " of "
          << parse->rules.size();
  VLOG(1) << "Found order: " << absl::StrJoin(field_order, ",");
  std::string_view prefix = "departure";
  int prefix_count = 0;
  int64_t product = 1;
  for (int i = 0; i < field_order.size(); ++i) {
    if (field_order[i].substr(0, prefix.size()) == prefix) {
      ++prefix_count;
      VLOG(1) << " field_order: " << field_order[i] << ": " << i;
      VLOG(1) << " *= " << parse->my_ticket[i];
      product *= parse->my_ticket[i];
    }
  }
  if (prefix_count != 6) return Error("Wrong number of prefix=", prefix);
  return AdventReturn(product);
}

}  // namespace advent_of_code
