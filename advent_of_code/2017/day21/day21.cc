#include "advent_of_code/2017/day21/day21.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

CharBoard Transform(const CharBoard& in, std::function<Point(Point)> trans) {
  CharBoard out = in;
  for (Point p : in.range()) {
    out[trans(p)] = in[p];
  }
  return out;
}

std::vector<std::function<Point(Point)>> Transforms(PointRectangle range) {
  std::vector<std::function<Point(Point)>> ret;
  ret.push_back([](Point p) { return Point{p.x, p.y}; });
  ret.push_back([range](Point p) { return Point{range.max.x - p.x, p.y}; });
  ret.push_back([range](Point p) { return Point{p.x, range.max.y - p.y}; });
  ret.push_back([range](Point p) {
    return Point{range.max.x - p.x, range.max.y - p.y};
  });
  ret.push_back([](Point p) { return Point{p.y, p.x}; });
  ret.push_back([range](Point p) { return Point{p.y, range.max.x - p.x}; });
  ret.push_back([range](Point p) { return Point{range.max.y - p.y, p.x}; });
  ret.push_back([range](Point p) {
    return Point{range.max.y - p.y, range.max.x - p.x};
  });
  return ret;
}

absl::StatusOr<CharBoard> FindPattern(
    const CharBoard& in,
    const absl::flat_hash_map<CharBoard, CharBoard>& patterns) {
  auto in_transforms = Transforms(in.range());
  for (int i = 0; i < in_transforms.size(); ++i) {
    CharBoard test = Transform(in, in_transforms[i]);
    if (auto it = patterns.find(test); it != patterns.end()) {
      return it->second;
      // auto out_transforms = Transforms(it->second.range());
      // return Transform(it->second, out_transforms[i]);
    }
  }
  return AdventDay::Error("No Patter found");
}

void SpliceBoard(CharBoard* out, const CharBoard& in, Point splice_at) {
  for (Point p : in.range()) {
    (*out)[p + splice_at] = in[p];
  }
}

absl::StatusOr<CharBoard> RunIteration(
    const CharBoard& in,
    const absl::flat_hash_map<CharBoard, CharBoard>& patterns) {
  Point stride;
  if (in.width() != in.height()) return AdventDay::Error("Not square");
  if (in.width() % 2 == 0) {
    stride = Point{2, 2};
  } else if (in.width() % 3 == 0) {
    stride = Point{3, 3};
  } else {
    return AdventDay::Error("Can't expand non mod-2, non mod-3 squares");
  }
  VLOG(2) << stride;
  CharBoard out(in.width() * (stride.x + 1) / stride.x,
                in.height() * (stride.y + 1) / stride.y);
  for (int y = 0; y < in.height() / stride.y; ++y) {
    for (int x = 0; x < in.width() / stride.x; ++x) {
      Point slice_at = {x * stride.x, y * stride.y};
      VLOG(2) << "Slice @" << slice_at;
      CharBoard slice = in.SubBoard(
          PointRectangle{slice_at, slice_at + stride - Point{1, 1}});
      VLOG(2) << "Slice:\n" << slice;
      absl::StatusOr<CharBoard> new_slice = FindPattern(slice, patterns);
      if (!new_slice.ok()) return new_slice.status();
      if (new_slice->width() != stride.x + 1)
        return AdventDay::Error("Bad pattern: x");
      if (new_slice->height() != stride.y + 1)
        return AdventDay::Error("Bad pattern: y");

      Point splice_at = {x * (stride.x + 1), y * (stride.y + 1)};
      VLOG(2) << "Splice @" << splice_at;
      SpliceBoard(&out, *new_slice, splice_at);
    }
  }
  return out;
}

absl::StatusOr<absl::flat_hash_map<CharBoard, CharBoard>> Parse(
    absl::Span<absl::string_view> input) {
  absl::flat_hash_map<CharBoard, CharBoard> ret;
  for (absl::string_view rule : input) {
    const auto [in, out] = AdventDay::PairSplit(rule, " => ");

    absl::StatusOr<CharBoard> board_in =
        CharBoard::Parse(absl::StrSplit(in, "/"));
    if (!board_in.ok()) return board_in.status();

    absl::StatusOr<CharBoard> board_out =
        CharBoard::Parse(absl::StrSplit(out, "/"));
    if (!board_out.ok()) return board_out.status();

    if (ret.contains(*board_in)) return AdventDay::Error("Duplicate pattern");
    // TODO(@monkeynova): Verify no rotations.

    ret.emplace(*board_in, *board_out);
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2017_21::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<absl::flat_hash_map<CharBoard, CharBoard>> patterns =
      Parse(input);
  if (!patterns.ok()) return patterns.status();

  std::vector<absl::string_view> init = {".#.", "..#", "###"};
  absl::StatusOr<CharBoard> tmp = CharBoard::Parse(absl::MakeSpan(init));
  if (!tmp.ok()) return tmp.status();

  VLOG(1) << "tmp=\n" << *tmp;
  for (int i = 0; i < 5; ++i) {
    absl::StatusOr<CharBoard> next = RunIteration(*tmp, *patterns);
    if (!next.ok()) return next.status();
    tmp = std::move(next);
    VLOG(1) << "tmp=\n" << *tmp;
  }

  int count = 0;
  for (Point p : tmp->range())
    if ((*tmp)[p] == '#') ++count;

  return IntReturn(count);
}

absl::StatusOr<std::string> Day_2017_21::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<absl::flat_hash_map<CharBoard, CharBoard>> patterns =
      Parse(input);
  if (!patterns.ok()) return patterns.status();

  std::vector<absl::string_view> init = {".#.", "..#", "###"};
  absl::StatusOr<CharBoard> tmp = CharBoard::Parse(absl::MakeSpan(init));
  if (!tmp.ok()) return tmp.status();

  VLOG(1) << "tmp=\n" << *tmp;
  for (int i = 0; i < 18; ++i) {
    absl::StatusOr<CharBoard> next = RunIteration(*tmp, *patterns);
    if (!next.ok()) return next.status();
    tmp = std::move(next);
    VLOG(1) << "tmp=\n" << *tmp;
  }

  int count = 0;
  for (Point p : tmp->range())
    if ((*tmp)[p] == '#') ++count;

  return IntReturn(count);
}

}  // namespace advent_of_code
