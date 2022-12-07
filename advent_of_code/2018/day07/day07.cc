// https://adventofcode.com/2018/day/7
//
// --- Day 7: The Sum of Its Parts ---
// -----------------------------------
//
// You find yourself standing on a snow-covered coastline; apparently,
// you landed a little off course. The region is too hilly to see the
// North Pole from here, but you do spot some Elves that seem to be
// trying to unpack something that washed ashore. It's quite cold out, so
// you decide to risk creating a paradox by asking them for directions.
//
// "Oh, are you the search party?" Somehow, you can understand whatever
// Elves from the year 1018 speak; you assume it's Ancient Nordic Elvish.
// Could the device on your wrist also be a translator? "Those clothes
// don't look very warm; take this." They hand you a heavy coat.
//
// "We do need to find our way back to the North Pole, but we have higher
// priorities at the moment. You see, believe it or not, this box
// contains something that will solve all of Santa's transportation
// problems - at least, that's what it looks like from the pictures in
// the instructions." It doesn't seem like they can read whatever
// language it's in, but you can: "Sleigh kit. Some assembly required."
//
// "'Sleigh'? What a wonderful name! You must help us assemble this
// 'sleigh' at once!" They start excitedly pulling more parts out of the
// box.
//
// The instructions specify a series of steps and requirements about
// which steps must be finished before others can begin (your puzzle
// input). Each step is designated by a single letter. For example,
// suppose you have the following instructions:
//
// Step C must be finished before step A can begin.
// Step C must be finished before step F can begin.
// Step A must be finished before step B can begin.
// Step A must be finished before step D can begin.
// Step B must be finished before step E can begin.
// Step D must be finished before step E can begin.
// Step F must be finished before step E can begin.
//
// Visually, these requirements look like this:
//
// -->A--->B--
// /    \      \
// C      -->D----->E
// \           /
// ---->F-----
//
// Your first goal is to determine the order in which the steps should be
// completed. If more than one step is ready, choose the step which is
// first alphabetically. In this example, the steps would be completed as
// follows:
//
// * Only C is available, and so it is done first.
//
// * Next, both A and F are available. A is first alphabetically, so it
// is done next.
//
// * Then, even though F was available earlier, steps B and D are now
// also available, and B is the first alphabetically of the three.
//
// * After that, only D and F are available. E is not available because
// only some of its prerequisites are complete. Therefore, D is
// completed next.
//
// * F is the only choice, so it is done next.
//
// * Finally, E is completed.
//
// So, in this example, the correct order is CABDFE.
//
// In what order should the steps in your instructions be completed?
//
// --- Part Two ---
// ----------------
//
// As you're about to begin construction, four of the Elves offer to
// help. "The sun will set soon; it'll go faster if we work together."
// Now, you need to account for multiple people working on steps
// simultaneously. If multiple steps are available, workers should still
// begin them in alphabetical order.
//
// Each step takes 60 seconds plus an amount corresponding to its letter:
// A=1, B=2, C=3, and so on. So, step A takes 60+1=61 seconds, while step
// Z takes 60+26=86 seconds. No time is required between steps.
//
// To simplify things for the example, however, suppose you only have
// help from one Elf (a total of two workers) and that each step takes 60
// fewer seconds (so that step A takes 1 second and step Z takes 26
// seconds). Then, using the same instructions as above, this is how each
// second would be spent:
//
// Second   Worker 1   Worker 2   Done
// 0        C          .
// 1        C          .
// 2        C          .
// 3        A          F       C
// 4        B          F       CA
// 5        B          F       CA
// 6        D          F       CAB
// 7        D          F       CAB
// 8        D          F       CAB
// 9        D          .       CABF
// 10        E          .       CABFD
// 11        E          .       CABFD
// 12        E          .       CABFD
// 13        E          .       CABFD
// 14        E          .       CABFD
// 15        .          .       CABFDE
//
// Each row represents one second of time. The Second column identifies
// how many seconds have passed as of the beginning of that second. Each
// worker column shows the step that worker is currently doing (or . if
// they are idle). The Done column shows completed steps.
//
// Note that the order of the steps has changed; this is because steps
// now take time to finish and multiple workers can begin multiple steps
// simultaneously.
//
// In this example, it would take 15 seconds for two workers to complete
// these steps.
//
// With 5 workers and the 60+ second step durations described above, how
// long will it take to complete all of the steps?

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
  absl::string_view item = "";
  int complete_time = 0;
};

struct WorkState {
  const DirectedGraph<bool>& graph;
  int time = 0;
  absl::flat_hash_set<absl::string_view> completed;
  absl::flat_hash_set<absl::string_view> to_do;
  absl::flat_hash_set<absl::string_view> available;
  std::vector<Worker> workers;
};

void FindNextAvailable(WorkState* work) {
  for (absl::string_view test : work->to_do) {
    const std::vector<absl::string_view>* incoming = work->graph.Incoming(test);
    bool can_add = true;
    if (incoming != nullptr) {
      for (absl::string_view in : *incoming) {
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
  for (absl::string_view next : work->available) {
    free_worker->item = next;
    free_worker->complete_time = work->time + 60 + next[0] - 'A' + 1;
    work->to_do.erase(next);
    work->available.erase(next);
    WorkState new_work = *work;
    int min_if = FindMinPathAssignWork(&new_work);
    min_to_complete = std::min(min_to_complete, min_if);
    work->available.insert(next);
    work->to_do.insert(next);
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
    absl::Span<absl::string_view> input) const {
  DirectedGraph<bool> graph;
  for (absl::string_view row : input) {
    absl::string_view src;
    absl::string_view dst;
    if (!RE2::FullMatch(
            row, "Step (.*) must be finished before step (.*) can begin.", &src,
            &dst)) {
      return Error("Bad input: ", row);
    }
    graph.AddEdge(src, dst);
  }
  absl::StatusOr<std::vector<absl::string_view>> ordered = graph.DAGSort(
      [](absl::string_view a, absl::string_view b) { return a < b; });
  if (!ordered.ok()) return ordered.status();
  return absl::StrJoin(*ordered, "");
}

absl::StatusOr<std::string> Day_2018_07::Part2(
    absl::Span<absl::string_view> input) const {
  DirectedGraph<bool> graph;
  for (absl::string_view row : input) {
    absl::string_view src;
    absl::string_view dst;
    if (!RE2::FullMatch(
            row, "Step (.*) must be finished before step (.*) can begin.", &src,
            &dst)) {
      return Error("Bad input: ", row);
    }
    graph.AddEdge(src, dst);
  }
  return IntReturn(FindMinPath(graph, 5));
}

}  // namespace advent_of_code
