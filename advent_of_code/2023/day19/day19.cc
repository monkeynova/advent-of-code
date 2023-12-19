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
  absl::string_view field;
  enum Type { kGt, kLt } cmp_type;
  int cmp;
  absl::string_view dest;
};

struct Possible {
  int xmin, xmax;
  int mmin, mmax;
  int amin, amax;
  int smin, smax;
  int64_t TotalScore() const {
    int64_t p = 1;
    p *= (xmax - xmin + 1);
    p *= (mmax - mmin + 1);
    p *= (amax - amin + 1);
    p *= (smax - smin + 1);
    return p;
  }
};

int64_t CountAllPossible(
    const absl::flat_hash_map<std::string_view, std::vector<Rule>>& rules,
    std::string_view state, Possible p) {
  if (p.xmin > p.xmax) return 0;
  if (p.mmin > p.mmax) return 0;
  if (p.amin > p.amax) return 0;
  if (p.smin > p.smax) return 0;

  if (state == "R") return 0;
  if (state == "A") {
    return p.TotalScore();
  }
  
  auto it = rules.find(state);
  CHECK(it != rules.end());
  int64_t total = 0;
  for (const Rule& r : it->second) {
    if (r.field == "") {
      total += CountAllPossible(rules, r.dest, p);
    } else if (r.field == "x") {
      if (r.cmp_type == Rule::kLt) {
        Possible sub = p;
        p.xmin = r.cmp;
        sub.xmax = r.cmp - 1;
        total += CountAllPossible(rules, r.dest, sub);
      } else if (r.cmp_type == Rule::kGt) {
        Possible sub = p;
        p.xmax = r.cmp;
        sub.xmin = r.cmp + 1;
        total += CountAllPossible(rules, r.dest, sub);
      }
    } else if (r.field == "m") {
      if (r.cmp_type == Rule::kLt) {
        Possible sub = p;
        p.mmin = r.cmp;
        sub.mmax = r.cmp - 1;
        total += CountAllPossible(rules, r.dest, sub);
      } else if (r.cmp_type == Rule::kGt) {
        Possible sub = p;
        p.mmax = r.cmp;
        sub.mmin = r.cmp + 1;
        total += CountAllPossible(rules, r.dest, sub);
      }
    } else if (r.field == "a") {
      if (r.cmp_type == Rule::kLt) {
        Possible sub = p;
        p.amin = r.cmp;
        sub.amax = r.cmp - 1;
        total += CountAllPossible(rules, r.dest, sub);
      } else if (r.cmp_type == Rule::kGt) {
        Possible sub = p;
        p.amax = r.cmp;
        sub.amin = r.cmp + 1;
        total += CountAllPossible(rules, r.dest, sub);
      }
    } else if (r.field == "s") {
      if (r.cmp_type == Rule::kLt) {
        Possible sub = p;
        p.smin = r.cmp;
        sub.smax = r.cmp - 1;
        total += CountAllPossible(rules, r.dest, sub);
      } else if (r.cmp_type == Rule::kGt) {
        Possible sub = p;
        p.smax = r.cmp;
        sub.smin = r.cmp + 1;
        total += CountAllPossible(rules, r.dest, sub);
      }
    }
  }
  return total;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_19::Part1(
    absl::Span<std::string_view> input) const {
  absl::flat_hash_map<std::string_view, std::vector<Rule>> rules;
  int total_score = 0;
  bool apply = false;
  for (std::string_view line : input) {
    if (line.empty()) {
      apply = true;
      continue;
    }
    if (!apply) {
      Tokenizer tok(line);
      std::string_view workflow = tok.Next();
      auto [it, inserted] = rules.emplace(workflow, std::vector<Rule>{});
      if (!inserted) return Error("Duplicate: ", workflow);
      RETURN_IF_ERROR(tok.NextIs("{"));
      while (true) {
        if (tok.Done()) return Error("Tokenizer ended early");
        Rule r;
        r.field = tok.Next();
        std::string_view cmp = tok.Next();
        if (cmp == "}") {
          if (!tok.Done()) return Error("Bad }");
          r.dest = r.field;
          r.field = "";
          it->second.push_back(r);
          break;
        } else if (cmp == ">") {
          r.cmp_type = Rule::kGt;
        } else if (cmp == "<") {
          r.cmp_type = Rule::kLt;
        }
        ASSIGN_OR_RETURN(r.cmp, tok.NextInt());
        RETURN_IF_ERROR(tok.NextIs(":"));
        r.dest = tok.Next();
        RETURN_IF_ERROR(tok.NextIs(","));
        it->second.push_back(r);
      }
    } else {
      absl::flat_hash_map<std::string_view, int> xmas;
      Tokenizer tok(line);
      RETURN_IF_ERROR(tok.NextIs("{"));
      while (true) {
        if (tok.Done()) return Error("Tokenizer ended early");
        std::string_view field = tok.Next();
        RETURN_IF_ERROR(tok.NextIs("="));
        ASSIGN_OR_RETURN(int val, tok.NextInt());
        xmas[field] = val;
        std::string_view delim = tok.Next();
        if (delim == "}") {
          if (!tok.Done()) return Error("Bad }");
          break;
        } else if (delim != ",") {
          return Error("Not ,");
        }
      }
      std::string_view state = "in";
      while (true) {
        if (state == "R") break;
        if (state == "A") {
          for (const auto& [k, v] : xmas) {
            total_score += v;
          }
          break;
        }
        auto it = rules.find(state);
        if (it == rules.end()) return Error("Bad state: ", state);
        std::string_view output;
        for (const Rule& r : it->second) {
          if (r.field.empty()) {
            output = r.dest;
            break;
          }
          if (r.cmp_type == Rule::kGt && xmas[r.field] > r.cmp) {
            output = r.dest;
            break;
          }
          if (r.cmp_type == Rule::kLt && xmas[r.field] < r.cmp) {
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
  absl::flat_hash_map<std::string_view, std::vector<Rule>> rules;
  for (std::string_view line : input) {
    if (line.empty()) {
      break;
    }
    Tokenizer tok(line);
    std::string_view workflow = tok.Next();
    auto [it, inserted] = rules.emplace(workflow, std::vector<Rule>{});
    if (!inserted) return Error("Duplicate: ", workflow);
    RETURN_IF_ERROR(tok.NextIs("{"));
    while (true) {
      if (tok.Done()) return Error("Tokenizer ended early");
      Rule r;
      r.field = tok.Next();
      std::string_view cmp = tok.Next();
      if (cmp == "}") {
        if (!tok.Done()) return Error("Bad }");
        r.dest = r.field;
        r.field = "";
        it->second.push_back(r);
        break;
      } else if (cmp == ">") {
        r.cmp_type = Rule::kGt;
      } else if (cmp == "<") {
        r.cmp_type = Rule::kLt;
      }
      ASSIGN_OR_RETURN(r.cmp, tok.NextInt());
      RETURN_IF_ERROR(tok.NextIs(":"));
      r.dest = tok.Next();
      RETURN_IF_ERROR(tok.NextIs(","));
      it->second.push_back(r);
    }
  }

  Possible all = {
    1, 4000, 1, 4000, 1, 4000, 1, 4000
  };
  
  return AdventReturn(CountAllPossible(rules, "in", all));
}

}  // namespace advent_of_code
