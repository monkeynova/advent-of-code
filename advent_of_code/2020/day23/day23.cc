#include "advent_of_code/2020/day23/day23.h"

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

class Cups {
 public:
  Cups() = default;

  void AddCup(int val);
  void Finalize();
  void RunMove2();

  absl::StatusOr<std::string> Label() const;
  absl::StatusOr<int64_t> Next2Product() const;

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Cups& c) {
    absl::Format(&sink, "1");
    auto it = c.cups_it_[1];
    for (++it; it != c.cups_it_[1];
         ++it == c.cups_.end() ? (it = c.cups_.begin()) : it) {
      if (it == c.cur_) {
        absl::Format(&sink, ",(%v)", *it);
      } else {
        absl::Format(&sink, ",%v", *it);
      }
    }
  }

 private:
  std::list<int> cups_;
  std::vector<std::list<int>::const_iterator> cups_it_;
  std::list<int>::const_iterator cur_;
};

void Cups::AddCup(int val) {
  cups_.push_back(val);
}

void Cups::Finalize() {
  cups_it_.resize(cups_.size() + 1, cups_.end());
  for (auto it = cups_.begin(); it != cups_.end(); ++it) {
    CHECK_LE(*it, cups_.size());
    cups_it_[*it] = it;
  }
  cur_ = cups_.begin();
}

void Cups::RunMove2() {
  int cur_cup = *cur_;
  int dest_cup = cur_cup - 1;
  if (dest_cup < 1) dest_cup = cups_.size();

  std::list<int> save3;
  {
    auto start = cur_;
    ++start;
    auto end = start;
    for (int i = 0; i < 3; ++i) {
      if (end == cups_.end()) {
        save3.splice(save3.end(), cups_, start, end);
        start = cups_.begin();
        end = start;
      }
      ++end;
    }
    save3.splice(save3.end(), cups_, start, end);
  }
  while(absl::c_any_of(
            save3, [dest_cup](int cup) { return cup == dest_cup; })) {
    --dest_cup;
    if (dest_cup < 1) dest_cup = cups_.size() + 3;
  }
  VLOG(2) << "  dest_cup=" << dest_cup;
  auto dest_it = cups_it_[dest_cup];
  // Splice inserts before.
  ++dest_it;
  cups_.splice(dest_it, save3);
  ++cur_;
  if (cur_ == cups_.end()) cur_ = cups_.begin();
}

absl::StatusOr<std::string> Cups::Label() const {
  std::string ret;
  int ret_i = 0;
  ret.resize(cups_.size() - 1);
  auto it = cups_it_[1];
  if (it == cups_.end()) return Error("Could not find 1");
  for (++it; it != cups_it_[1];
       ++it == cups_.end() ? (it = cups_.begin()) : it) {
    ret[ret_i] = *it + '0';
    ++ret_i;
  }
  return ret;
}

absl::StatusOr<int64_t> Cups::Next2Product() const {
  auto it = cups_it_[1];
  if (it == cups_.end()) return Error("Could not find 1");
  ++it;
  if (it == cups_.end()) it = cups_.begin();
  int64_t product = *it;
  ++it;
  if (it == cups_.end()) it = cups_.begin();
  product *= *it;
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
  cups.Finalize();
  for (int i = 0; i < 100; ++i) {
    VLOG(1) << cups;
    cups.RunMove2();
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
  for (int i = input[0].size(); i < 1000000; ++i) {
    cups.AddCup(i + 1);
  }
  cups.Finalize();
  for (int i = 0; i < 10'000'000; ++i) {
    VLOG_IF(1, i % 777'777 == 0) << i;
    cups.RunMove2();
  }

  return AdventReturn(cups.Next2Product());
}

}  // namespace advent_of_code
