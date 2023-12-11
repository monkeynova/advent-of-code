// http://adventofcode.com/2023/day/11

#include "advent_of_code/2023/day11/day11.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/graph_walk.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {


// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2023_11::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  absl::flat_hash_set<Point> galaxies_set = b.Find('#');
  std::vector<Point> galaxies(galaxies_set.begin(), galaxies_set.end());
  {
    std::vector<bool> x_hit(b.width(), false);
    for (Point p : galaxies) x_hit[p.x] = true;
    VLOG(1) << absl::StrJoin(x_hit, ",");
    for (Point& p : galaxies) {
      int delta_x = 0;
      for (int x = 0; x < p.x; ++x) {
        if (!x_hit[x]) ++delta_x;
      }
      VLOG(2) << p << " += {" << delta_x << ",0}";
      p.x += delta_x;
    }
  }
  {
    std::vector<bool> y_hit(b.height(), false);
    for (Point p : galaxies) y_hit[p.y] = true;
    VLOG(1) << absl::StrJoin(y_hit, ",");
    for (Point& p : galaxies) {
      int delta_y = 0;
      for (int y = 0; y < p.y; ++y) {
        if (!y_hit[y]) ++delta_y;
      }
      VLOG(2) << p << " += {0," << delta_y << "}";
      p.y += delta_y;
    }
  }
  int total_dist = 0;
  for (int i = 0; i < galaxies.size(); ++i) {
    Point p1 = galaxies[i];
    for (int j = i + 1; j < galaxies.size(); ++j) {
      Point p2 = galaxies[j];
      VLOG(2) << p1 << "-" << p2 << " = " << (p1 - p2).dist();
      total_dist += (p1 - p2).dist();
    }
  }
  return AdventReturn(total_dist);
}

absl::StatusOr<std::string> Day_2023_11::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(int64_t expand, IntParam());
  VLOG(1) << expand;
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  absl::flat_hash_set<Point> galaxies_set = b.Find('#');
  std::vector<Point> galaxies(galaxies_set.begin(), galaxies_set.end());
  int64_t test = b.width();
  test *= expand;
  CHECK_LT(test, std::numeric_limits<int>::max());
  {
    std::vector<bool> x_hit(b.width(), false);
    for (Point p : galaxies) x_hit[p.x] = true;
    VLOG(1) << absl::StrJoin(x_hit, ",");
    for (Point& p : galaxies) {
      int delta_x = 0;
      for (int x = 0; x < p.x; ++x) {
        if (!x_hit[x]) delta_x += expand - 1;
      }
      VLOG(2) << p << " += {" << delta_x << ",0}";
      p.x += delta_x;
    }
  }
  {
    std::vector<bool> y_hit(b.height(), false);
    for (Point p : galaxies) y_hit[p.y] = true;
    VLOG(1) << absl::StrJoin(y_hit, ",");
    for (Point& p : galaxies) {
      int delta_y = 0;
      for (int y = 0; y < p.y; ++y) {
        if (!y_hit[y]) delta_y += expand - 1;
      }
      VLOG(2) << p << " += {0," << delta_y << "}";
      p.y += delta_y;
    }
  }
  int64_t total_dist = 0;
  for (int i = 0; i < galaxies.size(); ++i) {
    Point p1 = galaxies[i];
    for (int j = i + 1; j < galaxies.size(); ++j) {
      Point p2 = galaxies[j];
      VLOG(2) << p1 << "-" << p2 << " = " << (p1 - p2).dist();
      total_dist += (p1 - p2).dist();
    }
  }
  return AdventReturn(total_dist);
}

}  // namespace advent_of_code
