// https://adventofcode.com/2018/day/22
//
// --- Day 22: Mode Maze ---
// -------------------------
//
// This is it, your final stop: the year -483. It's snowing and dark
// outside; the only light you can see is coming from a small cottage in
// the distance. You make your way there and knock on the door.
//
// A portly man with a large, white beard answers the door and invites
// you inside. For someone living near the North Pole in -483, he must
// not get many visitors, but he doesn't act surprised to see you.
// Instead, he offers you some milk and cookies.
//
// After talking for a while, he asks a favor of you. His friend hasn't
// come back in a few hours, and he's not sure where he is. Scanning the
// region briefly, you discover one life signal in a cave system nearby;
// his friend must have taken shelter there. The man asks if you can go
// there to retrieve his friend.
//
// The cave is divided into square regions which are either dominantly
// rocky, narrow, or wet (called its type). Each region occupies exactly
// one coordinate in X,Y format where X and Y are integers and zero or
// greater. (Adjacent regions can be the same type.)
//
// The scan (your puzzle input) is not very detailed: it only reveals the
// depth of the cave system and the coordinates of the target. However,
// it does not reveal the type of each region. The mouth of the cave is
// at 0,0.
//
// The man explains that due to the unusual geology in the area, there is
// a method to determine any region's type based on its erosion level.
// The erosion level of a region can be determined from its geologic
// index. The geologic index can be determined using the first rule that
// applies from the list below:
//
// * The region at 0,0 (the mouth of the cave) has a geologic index of
// 0.
//
// * The region at the coordinates of the target has a geologic index
// of 0.
//
// * If the region's Y coordinate is 0, the geologic index is its X
// coordinate times 16807.
//
// * If the region's X coordinate is 0, the geologic index is its Y
// coordinate times 48271.
//
// * Otherwise, the region's geologic index is the result of
// multiplying the erosion levels of the regions at X-1,Y and X,Y-1.
//
// A region's erosion level is its geologic index plus the cave system's
// depth, all modulo 20183. Then:
//
// * If the erosion level modulo 3 is 0, the region's type is rocky.
//
// * If the erosion level modulo 3 is 1, the region's type is wet.
//
// * If the erosion level modulo 3 is 2, the region's type is narrow.
//
// For example, suppose the cave system's depth is 510 and the target's
// coordinates are 10,10. Using % to represent the modulo operator, the
// cavern would look as follows:
//
// * At 0,0, the geologic index is 0. The erosion level is (0 + 510) %
// 20183 = 510. The type is 510 % 3 = 0, rocky.
//
// * At 1,0, because the Y coordinate is 0, the geologic index is 1 *
// 16807 = 16807. The erosion level is (16807 + 510) % 20183 = 17317.
// The type is 17317 % 3 = 1, wet.
//
// * At 0,1, because the X coordinate is 0, the geologic index is 1 *
// 48271 = 48271. The erosion level is (48271 + 510) % 20183 = 8415.
// The type is 8415 % 3 = 0, rocky.
//
// * At 1,1, neither coordinate is 0 and it is not the coordinate of
// the target, so the geologic index is the erosion level of 0,1 (8415)
// times the erosion level of 1,0 (17317), 8415 * 17317 = 145722555.
// The erosion level is (145722555 + 510) % 20183 = 1805. The type is
// 1805 % 3 = 2, narrow.
//
// * At 10,10, because they are the target's coordinates, the geologic
// index is 0. The erosion level is (0 + 510) % 20183 = 510. The type
// is 510 % 3 = 0, rocky.
//
// Drawing this same cave system with rocky as ., wet as =, narrow as |,
// the mouth as M, the target as T, with 0,0 in the top-left corner, X
// increasing to the right, and Y increasing downward, the top-left
// corner of the map looks like this:
//
// M   =.|=.|.|=.|=|=.
// .|=|=|||..|.=...
// .==|....||=..|==
// =.|....|.==.|==.
// =|..==...=.|==..
// =||.=.=||=|=..|=
// |.=.===|||..=..|
// |..==||=.|==|===
// .=..===..=|.|||.
// .======|||=|=.|=
// .===|=|===   T   ===||
// =|||...|==..|=.|
// =.=|=.=..=.||==|
// ||=|=...|==.=|==
// |=.=||===.|||===
// ||.|==.|.|.||=||
//
// Before you go in, you should determine the risk level of the area. For
// the rectangle that has a top-left corner of region 0,0 and a
// bottom-right corner of the region containing the target, add up the
// risk level of each individual region: 0 for rocky regions, 1 for wet
// regions, and 2 for narrow regions.
//
// In the cave system above, because the mouth is at 0,0 and the target
// is at 10,10, adding up the risk level of all regions with an X
// coordinate from 0 to 10 and a Y coordinate from 0 to 10, this total is
// 114.
//
// What is the total risk level for the smallest rectangle that includes
// 0,0 and the target's coordinates?
//
// --- Part Two ---
// ----------------
//
// Okay, it's time to go rescue the man's friend.
//
// As you leave, he hands you some tools: a torch and some climbing gear.
// You can't equip both tools at once, but you can choose to use neither.
//
// Tools can only be used in certain regions:
//
// * In rocky regions, you can use the climbing gear or the torch. You
// cannot use neither (you'll likely slip and fall).
//
// * In wet regions, you can use the climbing gear or neither tool. You
// cannot use the torch (if it gets wet, you won't have a light
// source).
//
// * In narrow regions, you can use the torch or neither tool. You
// cannot use the climbing gear (it's too bulky to fit).
//
// You start at 0,0 (the mouth of the cave) with the torch equipped and
// must reach the target coordinates as quickly as possible. The regions
// with negative X or Y are solid rock and cannot be traversed. The
// fastest route might involve entering regions beyond the X or Y
// coordinate of the target.
//
// You can move to an adjacent region (up, down, left, or right; never
// diagonally) if your currently equipped tool allows you to enter that
// region. Moving to an adjacent region takes one minute. (For example,
// if you have the torch equipped, you can move between rocky and narrow
// regions, but cannot enter wet regions.)
//
// You can change your currently equipped tool or put both away if your
// new equipment would be valid for your current region. Switching to
// using the climbing gear, torch, or neither always takes seven minutes,
// regardless of which tools you start with. (For example, if you are in
// a rocky region, you can switch from the torch to the climbing gear,
// but you cannot switch to neither.)
//
// Finally, once you reach the target, you need the torch equipped before
// you can find him in the dark. The target is always in a rocky region,
// so if you arrive there with climbing gear equipped, you will need to
// spend seven minutes switching to your torch.
//
// For example, using the same cave system as above, starting in the top
// left corner (0,0) and moving to the bottom right corner (the target,
// 10,10) as quickly as possible, one possible route is as follows, with
// your current position marked X:
//
// Initially:   X   =.|=.|.|=.|=|=.
// .|=|=|||..|.=...
// .==|....||=..|==
// =.|....|.==.|==.
// =|..==...=.|==..
// =||.=.=||=|=..|=
// |.=.===|||..=..|
// |..==||=.|==|===
// .=..===..=|.|||.
// .======|||=|=.|=
// .===|=|===T===||
// =|||...|==..|=.|
// =.=|=.=..=.||==|
// ||=|=...|==.=|==
// |=.=||===.|||===
// ||.|==.|.|.||=||
//
// Down:
// M=.|=.|.|=.|=|=.   X   |=|=|||..|.=...
// .==|....||=..|==
// =.|....|.==.|==.
// =|..==...=.|==..
// =||.=.=||=|=..|=
// |.=.===|||..=..|
// |..==||=.|==|===
// .=..===..=|.|||.
// .======|||=|=.|=
// .===|=|===T===||
// =|||...|==..|=.|
// =.=|=.=..=.||==|
// ||=|=...|==.=|==
// |=.=||===.|||===
// ||.|==.|.|.||=||
//
// Right:
// M=.|=.|.|=.|=|=.
// .   X   =|=|||..|.=...
// .==|....||=..|==
// =.|....|.==.|==.
// =|..==...=.|==..
// =||.=.=||=|=..|=
// |.=.===|||..=..|
// |..==||=.|==|===
// .=..===..=|.|||.
// .======|||=|=.|=
// .===|=|===T===||
// =|||...|==..|=.|
// =.=|=.=..=.||==|
// ||=|=...|==.=|==
// |=.=||===.|||===
// ||.|==.|.|.||=||
//
// Switch from using the torch to neither tool:
// M=.|=.|.|=.|=|=.
// .   X   =|=|||..|.=...
// .==|....||=..|==
// =.|....|.==.|==.
// =|..==...=.|==..
// =||.=.=||=|=..|=
// |.=.===|||..=..|
// |..==||=.|==|===
// .=..===..=|.|||.
// .======|||=|=.|=
// .===|=|===T===||
// =|||...|==..|=.|
// =.=|=.=..=.||==|
// ||=|=...|==.=|==
// |=.=||===.|||===
// ||.|==.|.|.||=||
//
// Right 3:
// M=.|=.|.|=.|=|=.
// .|=|   X   |||..|.=...
// .==|....||=..|==
// =.|....|.==.|==.
// =|..==...=.|==..
// =||.=.=||=|=..|=
// |.=.===|||..=..|
// |..==||=.|==|===
// .=..===..=|.|||.
// .======|||=|=.|=
// .===|=|===T===||
// =|||...|==..|=.|
// =.=|=.=..=.||==|
// ||=|=...|==.=|==
// |=.=||===.|||===
// ||.|==.|.|.||=||
//
// Switch from using neither tool to the climbing gear:
// M=.|=.|.|=.|=|=.
// .|=|   X   |||..|.=...
// .==|....||=..|==
// =.|....|.==.|==.
// =|..==...=.|==..
// =||.=.=||=|=..|=
// |.=.===|||..=..|
// |..==||=.|==|===
// .=..===..=|.|||.
// .======|||=|=.|=
// .===|=|===T===||
// =|||...|==..|=.|
// =.=|=.=..=.||==|
// ||=|=...|==.=|==
// |=.=||===.|||===
// ||.|==.|.|.||=||
//
// Down 7:
// M=.|=.|.|=.|=|=.
// .|=|=|||..|.=...
// .==|....||=..|==
// =.|....|.==.|==.
// =|..==...=.|==..
// =||.=.=||=|=..|=
// |.=.===|||..=..|
// |..==||=.|==|===
// .=..   X   ==..=|.|||.
// .======|||=|=.|=
// .===|=|===T===||
// =|||...|==..|=.|
// =.=|=.=..=.||==|
// ||=|=...|==.=|==
// |=.=||===.|||===
// ||.|==.|.|.||=||
//
// Right:
// M=.|=.|.|=.|=|=.
// .|=|=|||..|.=...
// .==|....||=..|==
// =.|....|.==.|==.
// =|..==...=.|==..
// =||.=.=||=|=..|=
// |.=.===|||..=..|
// |..==||=.|==|===
// .=..=   X   =..=|.|||.
// .======|||=|=.|=
// .===|=|===T===||
// =|||...|==..|=.|
// =.=|=.=..=.||==|
// ||=|=...|==.=|==
// |=.=||===.|||===
// ||.|==.|.|.||=||
//
// Down 3:
// M=.|=.|.|=.|=|=.
// .|=|=|||..|.=...
// .==|....||=..|==
// =.|....|.==.|==.
// =|..==...=.|==..
// =||.=.=||=|=..|=
// |.=.===|||..=..|
// |..==||=.|==|===
// .=..===..=|.|||.
// .======|||=|=.|=
// .===|=|===T===||
// =|||.   X   .|==..|=.|
// =.=|=.=..=.||==|
// ||=|=...|==.=|==
// |=.=||===.|||===
// ||.|==.|.|.||=||
//
// Right:
// M=.|=.|.|=.|=|=.
// .|=|=|||..|.=...
// .==|....||=..|==
// =.|....|.==.|==.
// =|..==...=.|==..
// =||.=.=||=|=..|=
// |.=.===|||..=..|
// |..==||=.|==|===
// .=..===..=|.|||.
// .======|||=|=.|=
// .===|=|===T===||
// =|||..   X   |==..|=.|
// =.=|=.=..=.||==|
// ||=|=...|==.=|==
// |=.=||===.|||===
// ||.|==.|.|.||=||
//
// Down:
// M=.|=.|.|=.|=|=.
// .|=|=|||..|.=...
// .==|....||=..|==
// =.|....|.==.|==.
// =|..==...=.|==..
// =||.=.=||=|=..|=
// |.=.===|||..=..|
// |..==||=.|==|===
// .=..===..=|.|||.
// .======|||=|=.|=
// .===|=|===T===||
// =|||...|==..|=.|
// =.=|=.   X   ..=.||==|
// ||=|=...|==.=|==
// |=.=||===.|||===
// ||.|==.|.|.||=||
//
// Right 4:
// M=.|=.|.|=.|=|=.
// .|=|=|||..|.=...
// .==|....||=..|==
// =.|....|.==.|==.
// =|..==...=.|==..
// =||.=.=||=|=..|=
// |.=.===|||..=..|
// |..==||=.|==|===
// .=..===..=|.|||.
// .======|||=|=.|=
// .===|=|===T===||
// =|||...|==..|=.|
// =.=|=.=..=   X   ||==|
// ||=|=...|==.=|==
// |=.=||===.|||===
// ||.|==.|.|.||=||
//
// Up 2:
// M=.|=.|.|=.|=|=.
// .|=|=|||..|.=...
// .==|....||=..|==
// =.|....|.==.|==.
// =|..==...=.|==..
// =||.=.=||=|=..|=
// |.=.===|||..=..|
// |..==||=.|==|===
// .=..===..=|.|||.
// .======|||=|=.|=
// .===|=|===   X   ===||
// =|||...|==..|=.|
// =.=|=.=..=.||==|
// ||=|=...|==.=|==
// |=.=||===.|||===
// ||.|==.|.|.||=||
//
// Switch from using the climbing gear to the torch:
// M=.|=.|.|=.|=|=.
// .|=|=|||..|.=...
// .==|....||=..|==
// =.|....|.==.|==.
// =|..==...=.|==..
// =||.=.=||=|=..|=
// |.=.===|||..=..|
// |..==||=.|==|===
// .=..===..=|.|||.
// .======|||=|=.|=
// .===|=|===   X   ===||
// =|||...|==..|=.|
// =.=|=.=..=.||==|
// ||=|=...|==.=|==
// |=.=||===.|||===
// ||.|==.|.|.||=||
//
// This is tied with other routes as the fastest way to reach the target:
// 45 minutes. In it, 21 minutes are spent switching tools (three times,
// seven minutes each) and the remaining 24 minutes are spent moving.
//
// What is the fewest number of minutes you can take to reach the target?

#include "advent_of_code/2018/day22/day22.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class Map {
 public:
  enum Terrain {
    kRocky = 0,
    kWet = 1,
    kNarrow = 2,
  };

  Map(int depth, Point target) : depth_(depth), target_(target) {}

  Point target() { return target_; }

  CharBoard ToCharBoard() {
    CharBoard ret(target_.x + 1, target_.y + 1);
    for (Point p : ret.range()) {
      Terrain terrain = GetTerrain(p);
      switch (terrain) {
        case Terrain::kRocky:
          ret[p] = '.';
          break;
        case Terrain::kWet:
          ret[p] = '=';
          break;
        case Terrain::kNarrow:
          ret[p] = '|';
          break;
        default:
          LOG(FATAL) << "Bad mod";
      }
    }
    return ret;
  }

  int GeologicIndex(Point p) {
    if (p == Point{0, 0}) return 0;
    if (p == target_) return 0;
    if (p.y == 0) return p.x * 16807;
    if (p.x == 0) return p.y * 48271;
    return ErosionLevel(p + Cardinal::kWest) *
           ErosionLevel(p + Cardinal::kNorth);
  }

  Terrain GetTerrain(Point p) {
    return static_cast<Terrain>(ErosionLevel(p) % 3);
  }

  int ErosionLevel(Point p) {
    if (auto it = erosion_.find(p); it != erosion_.end()) {
      return it->second;
    }
    int ret = (GeologicIndex(p) + depth_) % 20183;
    erosion_.emplace(p, ret);
    return ret;
  }

 private:
  int depth_;
  Point target_;
  absl::flat_hash_map<Point, int> erosion_;
};

class MapWalk : public BFSInterface<MapWalk> {
 public:
  enum Equipment {
    kNothing = 0,
    kTorch = 1,
    kGear = 2,
  };

  explicit MapWalk(Map* map)
      : map_(map), cur_(Point{0, 0}), equipped_(kTorch) {}

  const MapWalk& identifier() const override { return *this; }

  int min_steps_to_final() const override {
    return (cur_ - map_->target()).dist() + (equipped_ != kTorch ? 7 : 0);
  }

  bool IsFinal() const override {
    return cur_ == map_->target() && equipped_ == kTorch;
  }

  void AddNextSteps(State* state) const override {
    if (cur_ == map_->target() && equipped_ != kTorch) {
      MapWalk next = *this;
      // 7 to equip but one gets incremented with AddNextState.
      next.add_steps(6);
      next.equipped_ = kTorch;
      state->AddNextStep(next);
    }

    absl::flat_hash_set<Equipment> this_need =
        NeedEquip(map_->GetTerrain(cur_));
    for (Point dir : Cardinal::kFourDirs) {
      Point check = cur_ + dir;
      if (check.x < 0 || check.y < 0) continue;
      absl::flat_hash_set<Equipment> next_need =
          NeedEquip(map_->GetTerrain(check));
      for (Equipment e : next_need) {
        if (!this_need.contains(e)) continue;
        MapWalk next = *this;
        next.cur_ = check;
        next.equipped_ = e;
        if (next.equipped_ != equipped_) {
          next.add_steps(7);
        }
        state->AddNextStep(next);
      }
    }
  }

  absl::flat_hash_set<Equipment> NeedEquip(Map::Terrain terrain) const {
    switch (terrain) {
      case Map::Terrain::kRocky:
        return {kGear, kTorch};
      case Map::Terrain::kWet:
        return {kGear, kNothing};
      case Map::Terrain::kNarrow:
        return {kNothing, kTorch};
    }
    LOG(FATAL) << "Bad enum";
  }

  bool operator==(const MapWalk& o) const {
    return cur_ == o.cur_ && equipped_ == o.equipped_;
  }

  template <typename H>
  friend H AbslHashValue(H h, const MapWalk& m) {
    return H::combine(std::move(h), m.cur_, m.equipped_);
  }

  friend std::ostream& operator<<(std::ostream& o, const MapWalk& m) {
    return o << "{" << m.cur_ << ";" << m.equipped_ << "}";
  }

 private:
  Map* map_;
  Point cur_;
  Equipment equipped_;
};

}  // namespace

absl::StatusOr<std::string> Day_2018_22::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 2) return Error("Bad size");
  int depth;
  if (!RE2::FullMatch(input[0], "depth: (\\d+)", &depth)) {
    return Error("No depth");
  }
  Point target;
  if (!RE2::FullMatch(input[1], "target: (\\d+,\\d+)", target.Capture())) {
    return Error("No target");
  }
  Map map(depth, target);
  CharBoard b = map.ToCharBoard();
  // VLOG(1) << "Map:\n" << b;
  int risk = 0;
  for (Point p : b.range()) {
    if (b[p] == '=') ++risk;
    if (b[p] == '|') risk += 2;
  }
  return IntReturn(risk);
}

absl::StatusOr<std::string> Day_2018_22::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 2) return Error("Bad size");
  int depth;
  if (!RE2::FullMatch(input[0], "depth: (\\d+)", &depth)) {
    return Error("No depth");
  }
  Point target;
  if (!RE2::FullMatch(input[1], "target: (\\d+,\\d+)", target.Capture())) {
    return Error("No target");
  }
  Map map(depth, target);
  return IntReturn(MapWalk(&map).FindMinStepsAStar());
}

}  // namespace advent_of_code
