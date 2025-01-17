#include "advent_of_code/2018/day07/day07.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/directed_graph.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Worker {
  std::string_view item = "";
  int complete_time = 0;
};

struct WorkState {
  const DirectedGraph<bool>& graph;
  int time = 0;
  absl::flat_hash_set<std::string_view> completed;
  absl::flat_hash_set<std::string_view> to_do;
  absl::flat_hash_set<std::string_view> available;
  std::vector<Worker> workers;
};

void FindNextAvailable(WorkState* work) {
  for (std::string_view test : work->to_do) {
    const std::vector<std::string_view>* incoming = work->graph.Incoming(test);
    bool can_add = true;
    if (incoming != nullptr) {
      for (std::string_view in : *incoming) {
        if (!work->completed.contains(in)) can_add = false;
      }
    }
    if (can_add) work->available.insert(test);
  }
}

Worker* WaitForNextWorker(WorkState* work) {
  int complete_time = std::numeric_limits<int>::max();
  Worker* worker = nullptr;
  for (Worker& w : work->workers) {
    if (w.item != "" && w.complete_time < complete_time) {
      complete_time = w.complete_time;
      worker = &w;
    }
  }
  work->completed.insert(worker->item);
  worker->item = "";
  work->time = complete_time;
  return worker;
}

int FindMinPathAssignWork(WorkState* work) {
  if (work->to_do.empty()) {
    int complete_time = work->time;
    for (const Worker& w : work->workers) {
      complete_time = std::max(w.complete_time, complete_time);
    }
    return complete_time;
  }

  Worker* free_worker = nullptr;
  for (Worker& w : work->workers) {
    if (w.item == "") {
      free_worker = &w;
      break;
    }
  }
  if (free_worker == nullptr) {
    // No free worker. Advance to next time.
    free_worker = WaitForNextWorker(work);
  }

  while (work->available.empty()) {
    FindNextAvailable(work);
    if (work->available.empty()) {
      free_worker = WaitForNextWorker(work);
      CHECK(free_worker != nullptr);
    }
  }

  VLOG(1) << "Can assign @" << work->time << ": "
          << absl::StrJoin(work->available, ",");

  int min_to_complete = std::numeric_limits<int>::max();
  for (std::string_view next : work->available) {
    free_worker->item = next;
    free_worker->complete_time = work->time + 60 + next[0] - 'A' + 1;
    WorkState new_work = *work;
    new_work.to_do.erase(next);
    new_work.available.erase(next);
    int min_if = FindMinPathAssignWork(&new_work);
    min_to_complete = std::min(min_to_complete, min_if);
  }

  return min_to_complete;
}

int FindMinPath(const DirectedGraph<bool>& graph, int num_workers) {
  WorkState work{.graph = graph};
  work.workers.resize(num_workers);
  work.to_do = graph.nodes();
  return FindMinPathAssignWork(&work);
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2018_07::Part1(
    absl::Span<std::string_view> input) const {
  DirectedGraph<bool> graph;
  for (std::string_view row : input) {
    std::string_view src;
    std::string_view dst;
    if (!RE2::FullMatch(
            row, "Step (.*) must be finished before step (.*) can begin.", &src,
            &dst)) {
      return Error("Bad input: ", row);
    }
    graph.AddEdge(src, dst);
  }
  ASSIGN_OR_RETURN(std::vector<std::string_view> ordered,
                   graph.DAGSort([](std::string_view a, std::string_view b) {
                     return a < b;
                   }));
  return absl::StrJoin(ordered, "");
}

absl::StatusOr<std::string> Day_2018_07::Part2(
    absl::Span<std::string_view> input) const {
  DirectedGraph<bool> graph;
  for (std::string_view row : input) {
    std::string_view src;
    std::string_view dst;
    if (!RE2::FullMatch(
            row, "Step (.*) must be finished before step (.*) can begin.", &src,
            &dst)) {
      return Error("Bad input: ", row);
    }
    graph.AddEdge(src, dst);
  }
  return AdventReturn(FindMinPath(graph, 5));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2018, /*day=*/7,
    []() { return std::unique_ptr<AdventDay>(new Day_2018_07()); });

}  // namespace advent_of_code
