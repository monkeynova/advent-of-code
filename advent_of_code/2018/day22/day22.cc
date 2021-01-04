#include "advent_of_code/2018/day22/day22.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "glog/logging.h"
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

  bool IsFinal() override {
    return cur_ == map_->target() && equipped_ == kTorch;
  }

  void AddNextSteps(State* state) override {
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

  absl::flat_hash_set<Equipment> NeedEquip(Map::Terrain terrain) {
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

absl::StatusOr<std::vector<std::string>> Day22_2018::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 2) return Error("Bad size");
  int depth;
  if (!RE2::FullMatch(input[0], "depth: (\\d+)", &depth)) {
    return Error("No depth");
  }
  Point target;
  if (!RE2::FullMatch(input[1], "target: (\\d+),(\\d+)", &target.x,
                      &target.y)) {
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

absl::StatusOr<std::vector<std::string>> Day22_2018::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 2) return Error("Bad size");
  int depth;
  if (!RE2::FullMatch(input[0], "depth: (\\d+)", &depth)) {
    return Error("No depth");
  }
  Point target;
  if (!RE2::FullMatch(input[1], "target: (\\d+),(\\d+)", &target.x,
                      &target.y)) {
    return Error("No target");
  }
  Map map(depth, target);
  return IntReturn(MapWalk(&map).FindMinStepsAStar());
}

}  // namespace advent_of_code
