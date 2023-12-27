#include "advent_of_code/2017/day18/day18.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2017/vm.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2017_18::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(VM_2017 vm, VM_2017::Parse(input));
  vm.ExecuteToRecv();
  std::vector<int> send_queue = vm.ConsumeSendQueue();
  return AdventReturn(send_queue.back());
}

absl::StatusOr<std::string> Day_2017_18::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(VM_2017 vm, VM_2017::Parse(input));
  VM_2017 p0 = vm;
  p0.set_register("p", 0);
  VM_2017 p1 = vm;
  p1.set_register("p", 1);
  bool saw_send = true;
  int p1_sends = 0;
  while (saw_send) {
    saw_send = false;
    p0.ExecuteToRecv();
    std::vector<int> send_queue = p0.ConsumeSendQueue();
    VLOG(1) << "p0 send_queue: " << send_queue.size();
    saw_send |= !send_queue.empty();
    p1.AddRecvQueue(send_queue);
    p1.ExecuteToRecv();
    send_queue = p1.ConsumeSendQueue();
    VLOG(1) << "p1 send_queue: " << send_queue.size();
    p1_sends += send_queue.size();
    saw_send |= !send_queue.empty();
    p0.AddRecvQueue(send_queue);
  }
  return AdventReturn(p1_sends);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2017, /*day=*/18, []() {
  return std::unique_ptr<AdventDay>(new Day_2017_18());
});

}  // namespace advent_of_code
