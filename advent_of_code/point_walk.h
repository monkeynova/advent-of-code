#ifndef ADVENT_OF_CODE_POINT_WALK_H
#define ADVENT_OF_CODE_POINT_WALK_H

#include "absl/functional/function_ref.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"

namespace advent_of_code {

class PointWalk : public BFSInterface<PointWalk, Point> {
 public:
  struct Options {
    Point start;
    absl::FunctionRef<bool(Point)> is_good;
    absl::FunctionRef<bool(Point)> is_final;
  };

  PointWalk(Options options)
   : is_good_(options.is_good), is_final_(options.is_final),
     cur_(options.start) {}

  Point identifier() const override { return cur_; }
  bool IsFinal() const override { return is_final_(cur_); }
  void AddNextSteps(State* state) const override {
    for (Point d : Cardinal::kFourDirs) {
      Point n = cur_ + d;
      if (!is_good_(n)) continue;
      PointWalk next = *this;
      next.cur_ = n;
      state->AddNextStep(next);
    }
  }

 private:
  absl::FunctionRef<bool(Point)> is_good_;
  absl::FunctionRef<bool(Point)> is_final_;
  Point cur_;
};

class Point3Walk : public BFSInterface<Point3Walk, Point3> {
 public:
  struct Options {
    Point3 start;
    absl::FunctionRef<bool(Point3)> is_good;
    absl::FunctionRef<bool(Point3)> is_final;
  };

  Point3Walk(Options options)
    : is_good_(options.is_good), is_final_(options.is_final),
      cur_(options.start) {}

  Point3 identifier() const override { return cur_; }
  bool IsFinal() const override { return is_final_(cur_); }
  void AddNextSteps(State* state) const override {
    for (Point3 dir : Cardinal3::kSixDirs) {
      Point3 test = cur_ + dir;
      if (!is_good_(test)) continue;
      Point3Walk next = *this;
      next.cur_ = test;
      state->AddNextStep(std::move(next));
    }
  }

 private:
  absl::FunctionRef<bool(Point3)> is_good_;
  absl::FunctionRef<bool(Point3)> is_final_;
  Point3 cur_;
};

}  // namespace advent_of_code

#endif  // #define ADVENT_OF_CODE_POINT_WALK_H
