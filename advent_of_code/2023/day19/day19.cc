// http://adventofcode.com/2023/day/19

#include "advent_of_code/2023/day19/day19.h"

#include "absl/algorithm/container.h"
#include "absl/log/log.h"
#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

namespace {

struct Rule {
  int field;
  enum Type { kGt, kLt } cmp_type;
  int cmp;
  int dest;
};

using Workflow = std::pair<int, std::vector<Rule>>;
using WorkflowSet = std::vector<std::vector<Rule>>;

int WorkflowId(std::string_view name) {
  CHECK_LE(name.size(), 3);
  if (name.size() == 1) {
    if (name[0] == 'R') return -1;
    if (name[0] == 'A') return -2;
  }
  int id = 0;
  for (char c : name) {
    id = id * 26 + c - 'a';
  }
  return id;
}

absl::StatusOr<Workflow> ParseWorkflow(std::string_view line) {
  static const std::array<int, 128> kFieldMap = []() {
    std::array<int, 128> kFieldMap;
    for (int i = 0; i < kFieldMap.size(); ++i) kFieldMap[i] = -1;
    kFieldMap['x'] = 0;
    kFieldMap['m'] = 1;
    kFieldMap['a'] = 2;
    kFieldMap['s'] = 3;
    return kFieldMap;
  }();
  Workflow ret;
  Tokenizer tok(line);
  ret.first = WorkflowId(tok.Next());
  RETURN_IF_ERROR(tok.NextIs("{"));
  while (true) {
    if (tok.Done()) return Error("Tokenizer ended early");
    Rule r;
    std::string_view next = tok.Next();
    std::string_view cmp = tok.Next();
    if (cmp == "}") {
      if (!tok.Done()) return Error("Bad }");
      r.dest = WorkflowId(next);
      r.field = -1;
      ret.second.push_back(r);
      return ret;
    }
    if (next.size() != 1) return Error("Bad field");
    r.field = kFieldMap[next[0]];
    if (r.field == -1) return Error("Bad field");

    if (cmp == ">") {
      r.cmp_type = Rule::kGt;
    } else if (cmp == "<") {
      r.cmp_type = Rule::kLt;
    }
    ASSIGN_OR_RETURN(r.cmp, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs(":"));
    r.dest = WorkflowId(tok.Next());
    RETURN_IF_ERROR(tok.NextIs(","));
    ret.second.push_back(r);
  }
  LOG(FATAL) << "Left infinite loop";
}

struct Xmas {
  std::array<int, 4> fields;

  int Score() const { return absl::c_accumulate(fields, 0); }

  bool CheckRule(const Rule& r) const {
    if (r.field == -1) return true;
    if (r.cmp_type == Rule::kGt && fields[r.field] > r.cmp) return true;
    if (r.cmp_type == Rule::kLt && fields[r.field] < r.cmp) return true;
    return false;
  }
};

absl::StatusOr<Xmas> ParseXmas(std::string_view line) {
  Xmas xmas;
  Tokenizer tok(line);
  RETURN_IF_ERROR(tok.NextIs("{"));
  RETURN_IF_ERROR(tok.NextIs("x"));
  RETURN_IF_ERROR(tok.NextIs("="));
  ASSIGN_OR_RETURN(xmas.fields[0], tok.NextInt());
  RETURN_IF_ERROR(tok.NextIs(","));
  RETURN_IF_ERROR(tok.NextIs("m"));
  RETURN_IF_ERROR(tok.NextIs("="));
  ASSIGN_OR_RETURN(xmas.fields[1], tok.NextInt());
  RETURN_IF_ERROR(tok.NextIs(","));
  RETURN_IF_ERROR(tok.NextIs("a"));
  RETURN_IF_ERROR(tok.NextIs("="));
  ASSIGN_OR_RETURN(xmas.fields[2], tok.NextInt());
  RETURN_IF_ERROR(tok.NextIs(","));
  RETURN_IF_ERROR(tok.NextIs("s"));
  RETURN_IF_ERROR(tok.NextIs("="));
  ASSIGN_OR_RETURN(xmas.fields[3], tok.NextInt());
  RETURN_IF_ERROR(tok.NextIs("}"));
  if (!tok.Done()) return Error("Extra tokens");
  return xmas;
}

struct Possible {
  std::array<std::pair<int, int>, 4> ranges;

  bool Empty() const {
    return absl::c_any_of(
        ranges, [](std::pair<int, int> r) { return r.first > r.second; });
  }

  int64_t Count() const {
    return absl::c_accumulate(ranges, int64_t{1},
                              [](int64_t a, std::pair<int, int> r) {
                                return a * (r.second - r.first + 1);
                              });
  }
};

int64_t CountAllPossible(const WorkflowSet& workflow_set, int state,
                         Possible p) {
  static const int kAccept = WorkflowId("A");
  static const int kReject = WorkflowId("R");

  if (p.Empty()) return 0;

  if (state == kReject) return 0;
  if (state == kAccept) {
    return p.Count();
  }

  int64_t total = 0;
  for (const Rule& r : workflow_set[state]) {
    Possible sub = p;
    if (r.field != -1) {
      if (r.cmp_type == Rule::kLt) {
        p.ranges[r.field].first = r.cmp;
        sub.ranges[r.field].second = r.cmp - 1;
      } else if (r.cmp_type == Rule::kGt) {
        p.ranges[r.field].second = r.cmp;
        sub.ranges[r.field].first = r.cmp + 1;
      }
    }
    total += CountAllPossible(workflow_set, r.dest, sub);
  }
  return total;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_19::Part1(
    absl::Span<std::string_view> input) const {
  static const int kAccept = WorkflowId("A");
  static const int kReject = WorkflowId("R");
  static const int kStart = WorkflowId("in");
  static const int kInvalid = -3;

  WorkflowSet workflow_set(26 * 26 * 26);
  int total_score = 0;
  bool apply = false;
  for (std::string_view line : input) {
    if (line.empty()) {
      apply = true;
      continue;
    }
    if (!apply) {
      ASSIGN_OR_RETURN(Workflow workflow, ParseWorkflow(line));
      workflow_set[workflow.first] = std::move(workflow.second);
    } else {
      ASSIGN_OR_RETURN(Xmas xmas, ParseXmas(line));
      int state = kStart;
      while (true) {
        if (state == kReject) break;
        if (state == kAccept) {
          total_score += xmas.Score();
          break;
        }
        int output = kInvalid;
        for (const Rule& r : workflow_set[state]) {
          if (xmas.CheckRule(r)) {
            output = r.dest;
            break;
          }
        }
        if (output == kInvalid) return Error("No output");
        state = output;
      }
    }
  }

  return AdventReturn(total_score);
}

absl::StatusOr<std::string> Day_2023_19::Part2(
    absl::Span<std::string_view> input) const {
  WorkflowSet workflow_set(26 * 26 * 26);
  for (std::string_view line : input) {
    if (line.empty()) {
      break;
    }
    ASSIGN_OR_RETURN(Workflow workflow, ParseWorkflow(line));
    workflow_set[workflow.first] = std::move(workflow.second);
  }

  int state = WorkflowId("in");
  Possible all = {
      std::pair<int, int>{1, 4000}, {1, 4000}, {1, 4000}, {1, 4000}};

  return AdventReturn(CountAllPossible(workflow_set, state, all));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2023, /*day=*/19,
    []() { return std::unique_ptr<AdventDay>(new Day_2023_19()); });

}  // namespace advent_of_code
