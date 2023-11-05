#include "advent_of_code/2020/day23/day23.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/splice_ring.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class Cups {
 public:
  Cups() = default;

  void AddCup(int val);
  void RunMove();

  absl::StatusOr<std::string> Label() const;
  absl::StatusOr<int64_t> Next2Product() const;

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Cups& c) {
    absl::Format(&sink, "1");
    auto it = c.cups_.Find(1);
    auto start = it;
    for (++it; it != start; ++it) {
      if (it == c.cur_) {
        absl::Format(&sink, ",(%v)", *it);
      } else {
        absl::Format(&sink, ",%v", *it);
      }
    }
  }

 private:
  using RingType =  SpliceRing<int64_t, SpliceRingIndexType::kSparse>;
  RingType cups_;
  RingType::const_iterator cur_;
};

void Cups::AddCup(int val) {
  if (cur_ == RingType::const_iterator()) {
    cups_.InsertSomewhere(val);
    cur_ = cups_.SomePoint();
    return;
  }
  cups_.InsertBefore(cur_, val);
}

void Cups::RunMove() {
  std::array<int, 3> next3;
  {
    auto it = cur_;
    for (int i = 0; i < 3; ++i) {
      next3[i] = *++it;
    }
  }

  int dest_cup = *cur_;
  do {
    --dest_cup;
    if (dest_cup < 1) dest_cup = cups_.size();
  } while(absl::c_any_of(
              next3, [dest_cup](int cup) { return cup == dest_cup; }));
  VLOG(2) << "  dest_cup=" << dest_cup;

  auto dest_it = cups_.Find(dest_cup);
  // Splice inserts before.
  ++dest_it;
  auto src_it = cur_;
  ++src_it;
  for (int i = 0; i < 3; ++i) {
    src_it = cups_.MoveBefore(dest_it, src_it);
  }
  ++cur_;
}

absl::StatusOr<std::string> Cups::Label() const {
  std::string ret;
  int ret_i = 0;
  ret.resize(cups_.size() - 1);
  auto it = cups_.Find(1);
  auto start = it;
  for (++it; it != start; ++it) {
    ret[ret_i] = *it + '0';
    ++ret_i;
  }
  return ret;
}

absl::StatusOr<int64_t> Cups::Next2Product() const {
  auto it = cups_.Find(1);
  int64_t product = *++it;
  product *= *++it;
  return product;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_23::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");

  Cups cups;
  for (int i = 0; i < input[0].size(); ++i) {
    cups.AddCup(input[0][i] - '0');
  }
  for (int i = 0; i < 100; ++i) {
    VLOG(1) << cups;
    cups.RunMove();
  }
  VLOG(1) << cups;
  return AdventReturn(cups.Label());
}

absl::StatusOr<std::string> Day_2020_23::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  Cups cups;
  for (int i = 0; i < input[0].size(); ++i) {
    cups.AddCup(input[0][i] - '0');
  }
  for (int i = input[0].size(); i < 1'000'000; ++i) {
    cups.AddCup(i + 1);
  }
  for (int i = 0; i < 10'000'000; ++i) {
    VLOG_IF(1, i % 777'777 == 0) << i;
    cups.RunMove();
  }

  return AdventReturn(cups.Next2Product());
}

}  // namespace advent_of_code
