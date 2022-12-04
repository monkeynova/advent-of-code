// http://adventofcode.com/2016/day/17

#include "advent_of_code/2016/day17/day17.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/md5.h"
#include "advent_of_code/point.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class PathWalk : public BFSInterface<PathWalk> {
 public:
  static constexpr Point kKeyDirs[] = {Cardinal::kNorth, Cardinal::kSouth,
                                       Cardinal::kWest, Cardinal::kEast};
  static constexpr absl::string_view kDirNames = "UDLR";

  PathWalk(absl::string_view input, std::string* path_dest, bool stop_on_end)
      : input_(input),
        cur_(Point{0, 0}),
        path_dest_(path_dest),
        stop_on_end_(stop_on_end) {}

  bool IsFinal() const override {
    if (cur_ == Point{3, 3}) {
      *path_dest_ = path_;
      if (stop_on_end_) return true;
    }
    return false;
  }

  void AddNextSteps(State* state) const override {
    if (cur_ == Point{3, 3}) return;

    MD5 digest;
    absl::string_view md5 = digest.DigestHex(absl::StrCat(input_, path_));
    for (int i = 0; i < 4; ++i) {
      if (md5[i] >= 'b') {
        PathWalk next = *this;
        next.cur_ += kKeyDirs[i];
        next.path_.append(kDirNames.substr(i, 1));
        if (next.cur_.x < 0) continue;
        if (next.cur_.y < 0) continue;
        if (next.cur_.x > 3) continue;
        if (next.cur_.y > 3) continue;
        state->AddNextStep(next);
      }
    }
  }

  const PathWalk& identifier() const override { return *this; }

  bool operator==(const PathWalk& o) const {
    return cur_ == o.cur_ && path_ == o.path_;
  }

  template <typename H>
  friend H AbslHashValue(H h, const PathWalk& p) {
    return H::combine(std::move(h), p.path_, p.cur_);
  }

  friend std::ostream& operator<<(std::ostream& o, const PathWalk& p) {
    return o << p.path_ << "->" << p.cur_;
  }

 private:
  absl::string_view input_;
  std::string path_;
  Point cur_;
  std::string* path_dest_;
  bool stop_on_end_;
};

}  // namespace

absl::StatusOr<std::string> Day_2016_17::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Input size");
  std::string path;
  if (!PathWalk(input[0], &path, true).FindMinSteps()) {
    return Error("Path not found");
  }
  return path;
}

absl::StatusOr<std::string> Day_2016_17::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Input size");
  std::string path;
  // We walk until exhausting paths.
  (void)PathWalk(input[0], &path, false).FindMinSteps();
  return IntReturn(path.size());
}

}  // namespace advent_of_code
