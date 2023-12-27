#include "advent_of_code/2015/day18/day18.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/conway.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class ForceCorners : public ConwayBoard {
 public:
  explicit ForceCorners(const CharBoard& b) : ConwayBoard(b) {
    corners_[0] = bounds().min;
    corners_[1] = bounds().max;
    corners_[2].x = bounds().min.x;
    corners_[2].y = bounds().max.y;
    corners_[3].x = bounds().max.x;
    corners_[3].y = bounds().min.y;
    for (Point p : corners_) Force(p);
  };

  bool Advance() override {
    bool ret = ConwayBoard::Advance();
    for (Point p : corners_) Force(p);
    return ret;
  }

 private:
  std::array<Point, 4> corners_;
};

}  // namespace

absl::StatusOr<std::string> Day_2015_18::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard board, CharBoard::Parse(input));

  ConwayBoard conway(board);
  conway.AdvanceN(100);

  return AdventReturn(conway.CountLive());
}

absl::StatusOr<std::string> Day_2015_18::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard board, CharBoard::Parse(input));

  ForceCorners conway(board);
  conway.AdvanceN(100);
  return AdventReturn(conway.CountLive());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2015, /*day=*/18, []() {
  return std::unique_ptr<AdventDay>(new Day_2015_18());
});

}  // namespace advent_of_code
