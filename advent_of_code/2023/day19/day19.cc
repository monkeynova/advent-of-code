// http://adventofcode.com/2023/day/19

#include "advent_of_code/2023/day19/day19.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/graph_walk.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Rule {
  int field;
  enum Type { kGt, kLt } cmp_type;
  int cmp;
  absl::string_view dest;
};

using Workflow = std::pair<std::string_view, std::vector<Rule>>;
using WorkflowSet = absl::flat_hash_map<std::string_view, std::vector<Rule>>;

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
  ret.first = tok.Next();
  RETURN_IF_ERROR(tok.NextIs("{"));
  while (true) {
    if (tok.Done()) return Error("Tokenizer ended early");
    Rule r;
    r.dest = tok.Next();
    std::string_view cmp = tok.Next();
    if (cmp == "}") {
      if (!tok.Done()) return Error("Bad }");
      r.field = -1;
      ret.second.push_back(r);
      return ret;
    }
    if (r.dest.size() != 1) return Error("Bad field");
    r.field = kFieldMap[r.dest[0]];
    if (r.field == -1) return Error("Bad field");
    
    if (cmp == ">") {
      r.cmp_type = Rule::kGt;
    } else if (cmp == "<") {
      r.cmp_type = Rule::kLt;
    }
    ASSIGN_OR_RETURN(r.cmp, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs(":"));
    r.dest = tok.Next();
    RETURN_IF_ERROR(tok.NextIs(","));
    ret.second.push_back(r);
  }
  LOG(FATAL) << "Left infinite loop";
}

struct Xmas {
  std::array<int, 4> fields;

  int Score() const {
    return absl::c_accumulate(fields, 0);
  }

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
      ranges, [](std::pair<int, int> r) {
        return r.first > r.second;
      });
  }

  int64_t TotalScore() const {
    return absl::c_accumulate(
      ranges, int64_t{1},
      [](int64_t a, std::pair<int, int> r) {
        return a * (r.second - r.first + 1);
      });
  }
};

int64_t CountAllPossible(
    const WorkflowSet& workflow_set,
    std::string_view state, Possible p) {
  if (p.Empty()) return 0;
  
  if (state == "R") return 0;
  if (state == "A") {
    return p.TotalScore();
  }
  
  auto it = workflow_set.find(state);
  CHECK(it != workflow_set.end());
  int64_t total = 0;
  for (const Rule& r : it->second) {
    if (r.field == -1) {
      total += CountAllPossible(workflow_set, r.dest, p);
    } else {
      Possible sub = p;
      if (r.cmp_type == Rule::kLt) {
        p.ranges[r.field].first = r.cmp;
        sub.ranges[r.field].second = r.cmp - 1;
      } else if (r.cmp_type == Rule::kGt) {
        p.ranges[r.field].second = r.cmp;
        sub.ranges[r.field].first = r.cmp + 1;
      }      
      total += CountAllPossible(workflow_set, r.dest, sub);
    }
  }
  return total;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_19::Part1(
    absl::Span<std::string_view> input) const {
  WorkflowSet workflow_set;
  int total_score = 0;
  bool apply = false;
  for (std::string_view line : input) {
    if (line.empty()) {
      apply = true;
      continue;
    }
    if (!apply) {
      Workflow workflow;
      ASSIGN_OR_RETURN(workflow, ParseWorkflow(line));
      auto [it, inserted] = workflow_set.insert(std::move(workflow));
      if (!inserted) return Error("Duplicate: ", workflow.first);

    } else {
      ASSIGN_OR_RETURN(Xmas xmas, ParseXmas(line));
      std::string_view state = "in";
      while (true) {
        if (state == "R") break;
        if (state == "A") {
          total_score += xmas.Score();
          break;
        }
        auto it = workflow_set.find(state);
        if (it == workflow_set.end()) return Error("Bad state: ", state);
        std::string_view output;
        for (const Rule& r : it->second) {
          if (xmas.CheckRule(r)) {
            output = r.dest;
            break;
          }
        }
        if (output.empty()) return Error("No output");
        state = output;
      }
    }
  }
  
  return AdventReturn(total_score);
}

absl::StatusOr<std::string> Day_2023_19::Part2(
    absl::Span<std::string_view> input) const {
  WorkflowSet workflow_set;
  for (std::string_view line : input) {
    if (line.empty()) {
      break;
    }
    Workflow workflow;
    ASSIGN_OR_RETURN(workflow, ParseWorkflow(line));
    auto [it, inserted] = workflow_set.insert(std::move(workflow));
    if (!inserted) return Error("Duplicate: ", workflow.first);
  }

  Possible all = {
    std::pair<int, int>{1, 4000}, {1, 4000}, {1, 4000}, {1, 4000}
  };
  
  return AdventReturn(CountAllPossible(workflow_set, "in", all));
}

}  // namespace advent_of_code
