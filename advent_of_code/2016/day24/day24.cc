#include "advent_of_code/2016/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/bfs.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class PathWalk : public BFSInterface<PathWalk> {
 public:
  PathWalk(const CharBoard& board, bool back_to_origin)
   : board_(board),
     start_(FindStart(board)),
     cur_(start_),
     need_keys_(NeedKeys(board)),
     have_keys_(0),
     back_to_origin_(back_to_origin) {}

  void AddNextSteps(State* state) final {
    for (Point dir : Cardinal::kFourDirs) {
      Point next_point = cur_ + dir;
      if (board_.OnBoard(next_point) && board_[next_point] != '#') {
        PathWalk next = *this;
        next.cur_ = next_point;
        if (board_[next_point] > '0' && board_[next_point] <= '9') {
          next.have_keys_ |= 1 << (board_[next_point] - '1');
        }
        state->AddNextStep(std::move(next));
      }
    }
  }

  bool IsFinal() final {
    if (need_keys_ != have_keys_) return false;
    if (back_to_origin_ && cur_ != start_) return false;
    return true;
  }

  bool operator==(const PathWalk& o) const {
    return cur_ == o.cur_ && have_keys_ == o.have_keys_;
  }

  template <typename H>
  friend H AbslHashValue(H h, const PathWalk& pw) {
    return H::combine(std::move(h), pw.cur_, pw.have_keys_);
  }

  friend std::ostream& operator<<(std::ostream& o, const PathWalk& pw) {
    return o << "{" << pw.cur_ << "," << std::hex << pw.have_keys_ << std::dec
             << "}";
  }

 private:
  Point FindStart(const CharBoard& b) {
    Point start;
    for (Point p : b.range()) {
      if (b[p] == '0') start = p;
    }
    return start;
  }
  
  int NeedKeys(const CharBoard& b) {
    int need_keys = 0;
    for (Point p : b.range()) {
      if (b[p] > '0' && b[p] <= '9') {
        need_keys |= 1 << (b[p] - '1');
      }
    }
    return need_keys;
  }

  const CharBoard& board_;
  Point start_;
  Point cur_;
  int need_keys_;
  int have_keys_;
  bool back_to_origin_;
};

}  // namespace

absl::StatusOr<std::vector<std::string>> Day24_2016::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  return IntReturn(PathWalk(*b, false).FindMinSteps());
}

absl::StatusOr<std::vector<std::string>> Day24_2016::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  return IntReturn(PathWalk(*b, true).FindMinSteps());
}

}  // namespace advent_of_code
