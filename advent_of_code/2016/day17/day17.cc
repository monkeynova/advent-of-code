// https://adventofcode.com/2016/day/17
//
// --- Day 17: Two Steps Forward ---
// ---------------------------------
//
// You're trying to access a secure vault protected by a 4x4 grid of
// small rooms connected by doors. You start in the top-left room (marked
// S), and you can access the vault (marked V) once you reach the
// bottom-right room:
//
// #########
// #S| | | #
// #-#-#-#-#
// # | | | #
// #-#-#-#-#
// # | | | #
// #-#-#-#-#
// # | | |
// ####### V
//
// Fixed walls are marked with #, and doors are marked with - or |.
//
// The doors in your current room are either open or closed (and locked)
// based on the hexadecimal MD5 hash of a passcode (your puzzle input)
// followed by a sequence of uppercase characters representing the path
// you have taken so far (U for up, D for down, L for left, and R for
// right).
//
// Only the first four characters of the hash are used; they represent,
// respectively, the doors up, down, left, and right from your current
// position. Any b, c, d, e, or f means that the corresponding door is
// open; any other character (any number or a) means that the
// corresponding door is closed and locked.
//
// To access the vault, all you need to do is reach the bottom-right
// room; reaching this room opens the vault and all doors in the maze.
//
// For example, suppose the passcode is hijkl. Initially, you have taken
// no steps, and so your path is empty: you simply find the MD5 hash of
// hijkl alone. The first four characters of this hash are ced9, which
// indicate that up is open (c), down is open (e), left is open (d), and
// right is closed and locked (9). Because you start in the top-left
// corner, there are no "up" or "left" doors to be open, so your only
// choice is down.
//
// Next, having gone only one step (down, or D), you find the hash of
// hijklD. This produces f2bc, which indicates that you can go back up,
// left (but that's a wall), or right. Going right means hashing hijklDR
// to get 5745 - all doors closed and locked. However, going up instead
// is worthwhile: even though it returns you to the room you started in,
// your path would then be DU, opening a different set of doors.
//
// After going DU (and then hashing hijklDU to get 528e), only the right
// door is open; after going DUR, all doors lock. (Fortunately, your
// actual passcode is not hijkl).
//
// Passcodes actually used by Easter Bunny Vault Security do allow access
// to the vault if you know the right path. For example:
//
// * If your passcode were ihgpwlah, the shortest path would be DDRRRD.
//
// * With kglvqrro, the shortest path would be DDUDRLRRUDRD.
//
// * With ulqzkmiv, the shortest would be
// DRURDRUDDLLDLUURRDULRLDUUDDDRR.
//
// Given your vault's passcode, what is the shortest path (the actual
// path, not just the length) to reach the vault?
//
// --- Part Two ---
// ----------------
//
// You're curious how robust this security solution really is, and so you
// decide to find longer and longer paths which still provide access to
// the vault. You remember that paths always end the first time they
// reach the bottom-right room (that is, they can never pass through it,
// only end in it).
//
// For example:
//
// * If your passcode were ihgpwlah, the longest path would take 370
// steps.
//
// * With kglvqrro, the longest path would be 492 steps long.
//
// * With ulqzkmiv, the longest path would be 830 steps long.
//
// What is the length of the longest path that reaches the vault?

#include "advent_of_code/2016/day17/day17.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/md5.h"
#include "advent_of_code/point.h"
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
