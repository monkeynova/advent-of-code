// http://adventofcode.com/2022/day/13

#include "advent_of_code/2022/day13/day13.h"

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

struct Packet {
  bool IsInt() const { return absl::holds_alternative<int>(val); }
  int AsInt() const { return absl::get<int>(val); }
  const std::vector<Packet>& AsList() const {
    return absl::get<std::vector<Packet>>(val);
  }

  Packet& SetInt(int new_val) {
    val = new_val;
    return *this;
  }
  Packet& SetList(std::vector<Packet> new_list) {
    val = std::move(new_list);
    return *this;
  }

  absl::variant<int, std::vector<Packet>> val;

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Packet& cmp) {
    if (cmp.IsInt()) return absl::Format(&sink, "%v", cmp.AsInt());
    absl::Format(&sink, "[");
    for (int i = 0; i < cmp.AsList().size(); ++i) {
      if (i > 0) absl::Format(&sink, ",");
      absl::Format(&sink, "%v", cmp.AsList()[i]);
    }
    absl::Format(&sink, "]");
  }

  bool operator==(const Packet& o) const { return cmp(o) == 0; }

  bool operator<(const Packet& o) const { return cmp(o) < 0; }

  int cmp(const Packet& o) const {
    if (IsInt() && o.IsInt()) {
      return AsInt() < o.AsInt() ? -1 : AsInt() > o.AsInt() ? 1 : 0;
    }
    if (IsInt()) {
      Packet list_val;
      list_val.SetList({Packet().SetInt(AsInt())});
      return list_val.cmp(o);
    }
    if (o.IsInt()) {
      Packet list_val;
      list_val.SetList({Packet().SetInt(o.AsInt())});
      return cmp(list_val);
    }
    for (int i = 0; i < AsList().size(); ++i) {
      if (i >= o.AsList().size()) return 1;
      int sub_cmp = AsList()[i].cmp(o.AsList()[i]);
      if (sub_cmp != 0) return sub_cmp;
    }
    if (o.AsList().size() > AsList().size()) return -1;
    return 0;
  }
};

Packet Parse(std::string_view& line) {
  Packet ret;
  if (line[0] == '[') {
    line = line.substr(1);
    std::vector<Packet> list;
    while (true) {
      if (line[0] == ']') {
        line = line.substr(1);
        break;
      }
      list.push_back(Parse(line));
      if (line[0] == ',') line = line.substr(1);
    }
    ret.SetList(list);
  } else if (line[0] >= '0' && line[0] <= '9') {
    int val = 0;
    while (line[0] >= '0' && line[0] <= '9') {
      val = val * 10 + line[0] - '0';
      line = line.substr(1);
    }
    ret.SetInt(val);
  }
  return ret;
}

absl::StatusOr<Packet> ParseFull(std::string_view line) {
  std::string_view tmp = line;
  Packet ret = Parse(tmp);
  if (!tmp.empty()) return Error("Bad line: ", line);
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_13::Part1(
    absl::Span<std::string_view> input) const {
  int ret = 0;
  for (int i = 0; i < input.size(); i += 3) {
    if (!input[i + 2].empty()) return Error("Bad input: empty");
    ASSIGN_OR_RETURN(Packet left, ParseFull(input[i]));
    ASSIGN_OR_RETURN(Packet right, ParseFull(input[i + 1]));
    if (!(right < left)) ret += (i / 3) + 1;
  }
  return AdventReturn(ret);
}

absl::StatusOr<std::string> Day_2022_13::Part2(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<Packet> m1 = ParseFull("[[2]]");
  absl::StatusOr<Packet> m2 = ParseFull("[[6]]");

  int idx1 = 1;
  int idx2 = 2;
  for (int i = 0; i < input.size(); ++i) {
    if (input[i].empty()) continue;
    ASSIGN_OR_RETURN(Packet test, ParseFull(input[i]));
    if (test < *m1) {
      ++idx1;
      ++idx2;
    } else if (test < *m2) {
      ++idx2;
    }
  }
  return AdventReturn(idx1 * idx2);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2022, /*day=*/13,
    []() { return std::unique_ptr<AdventDay>(new Day_2022_13()); });

}  // namespace advent_of_code
