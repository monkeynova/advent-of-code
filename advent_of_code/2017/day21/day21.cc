// https://adventofcode.com/2017/day/21
//
// --- Day 21: Fractal Art ---
// ---------------------------
//
// You find a program trying to generate some art. It uses a strange
// process that involves repeatedly enhancing the detail of an image
// through a set of rules.
//
// The image consists of a two-dimensional square grid of pixels that are
// either on (#) or off (.). The program always begins with this pattern:
//
// .#.
// ..#
// ###
//
// Because the pattern is both 3 pixels wide and 3 pixels tall, it is
// said to have a size of 3.
//
// Then, the program repeats the following process:
//
// * If the size is evenly divisible by 2, break the pixels up into 2x2
// squares, and convert each 2x2 square into a 3x3 square by
// following the corresponding enhancement rule.
//
// * Otherwise, the size is evenly divisible by 3; break the pixels up
// into 3x3 squares, and convert each 3x3 square into a 4x4 square by
// following the corresponding enhancement rule.
//
// Because each square of pixels is replaced by a larger one, the image
// gains pixels and so its size increases.
//
// The artist's book of enhancement rules is nearby (your puzzle input);
// however, it seems to be missing rules. The artist explains that
// sometimes, one must rotate or flip the input pattern to find a match.
// (Never rotate or flip the output pattern, though.) Each pattern is
// written concisely: rows are listed as single units, ordered top-down,
// and separated by slashes. For example, the following rules correspond
// to the adjacent patterns:
//
// ../.#  =  ..
// .#
//
// .#.
// .#./..#/###  =  ..#
// ###
//
// #..#
// #..#/..../#..#/.##.  =  ....
// #..#
// .##.
//
// When searching for a rule to use, rotate and flip the pattern as
// necessary. For example, all of the following patterns match the same
// rule:
//
// .#.   .#.   #..   ###
// ..#   #..   #.#   ..#
// ###   ###   ##.   .#.
//
// Suppose the book contained the following two rules:
//
// ../.# => ##./#../...
// .#./..#/### => #..#/..../..../#..#
//
// As before, the program begins with this pattern:
//
// .#.
// ..#
// ###
//
// The size of the grid (3) is not divisible by 2, but it is divisible by
// 3. It divides evenly into a single square; the square matches the
// second rule, which produces:
//
// #..#
// ....
// ....
// #..#
//
// The size of this enhanced grid (4) is evenly divisible by 2, so that
// rule is used. It divides evenly into four squares:
//
// #.|.#
// ..|..
// --+--
// ..|..
// #.|.#
//
// Each of these squares matches the same rule (../.# => ##./#../...),
// three of which require some flipping and rotation to line up with the
// rule. The output for the rule is the same in all four cases:
//
// ##.|##.
// #..|#..
// ...|...
// ---+---
// ##.|##.
// #..|#..
// ...|...
//
// Finally, the squares are joined into a new grid:
//
// ##.##.
// #..#..
// ......
// ##.##.
// #..#..
// ......
//
// Thus, after 2 iterations, the grid contains 12 pixels that are on.
//
// How many pixels stay on after 5 iterations?
//
// --- Part Two ---
// ----------------
//
// How many pixels stay on after 18 iterations?

// http://adventofcode.com/2017/day/21

#include "advent_of_code/2017/day21/day21.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
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
  return Error("No Patter found");
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
  if (in.width() != in.height()) return Error("Not square");
  if (in.width() % 2 == 0) {
    stride = Point{2, 2};
  } else if (in.width() % 3 == 0) {
    stride = Point{3, 3};
  } else {
    return Error("Can't expand non mod-2, non mod-3 squares");
  }
  VLOG(2) << stride;
  CharBoard out(in.width() * (stride.x + 1) / stride.x,
                in.height() * (stride.y + 1) / stride.y);
  for (int y = 0; y < in.height() / stride.y; ++y) {
    for (int x = 0; x < in.width() / stride.x; ++x) {
      Point slice_at = {x * stride.x, y * stride.y};
      VLOG(2) << "Slice @" << slice_at;
      absl::StatusOr<CharBoard> slice = in.SubBoard(
          PointRectangle{slice_at, slice_at + stride - Point{1, 1}});
      if (!slice.ok()) return slice.status();
      VLOG(2) << "Slice:\n" << *slice;
      absl::StatusOr<CharBoard> new_slice = FindPattern(*slice, patterns);
      if (!new_slice.ok()) return new_slice.status();
      if (new_slice->width() != stride.x + 1) return Error("Bad pattern: x");
      if (new_slice->height() != stride.y + 1) return Error("Bad pattern: y");

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
    const auto [in, out] = PairSplit(rule, " => ");

    absl::StatusOr<CharBoard> board_in =
        CharBoard::Parse(absl::StrSplit(in, "/"));
    if (!board_in.ok()) return board_in.status();

    absl::StatusOr<CharBoard> board_out =
        CharBoard::Parse(absl::StrSplit(out, "/"));
    if (!board_out.ok()) return board_out.status();

    if (ret.contains(*board_in)) return Error("Duplicate pattern");
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

  return IntReturn(tmp->CountOn());
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

  return IntReturn(tmp->CountOn());
}

}  // namespace advent_of_code
