// https://adventofcode.com/2020/day/20
//
// --- Day 20: Jurassic Jigsaw ---
// -------------------------------
// 
// The high-speed train leaves the forest and quickly carries you south.
// You can even see a desert in the distance! Since you have some spare
// time, you might as well see if there was anything interesting in the
// image the Mythical Information Bureau satellite captured.
// 
// After decoding the satellite messages, you discover that the data
// actually contains many small images created by the satellite's camera
// array. The camera array consists of many cameras; rather than produce
// a single square image, they produce many smaller square image tiles
// that need to be reassembled back into a single image.
// 
// Each camera in the camera array returns a single monochrome image tile
// with a random unique ID number. The tiles (your puzzle input) arrived
// in a random order.
// 
// Worse yet, the camera array appears to be malfunctioning: each image
// tile has been rotated and flipped to a random orientation. Your first
// task is to reassemble the original image by orienting the tiles so
// they fit together.
// 
// To show how the tiles should be reassembled, each tile's image data
// includes a border that should line up exactly with its adjacent tiles.
// All tiles have this border, and the border lines up exactly when the
// tiles are both oriented correctly. Tiles at the edge of the image also
// have this border, but the outermost edges won't line up with any other
// tiles.
// 
// For example, suppose you have the following nine tiles:
// 
// Tile 2311:
// ..##.#..#.
// ##..#.....
// #...##..#.
// ####.#...#
// ##.##.###.
// ##...#.###
// .#.#.#..##
// ..#....#..
// ###...#.#.
// ..###..###
// 
// Tile 1951:
// #.##...##.
// #.####...#
// .....#..##
// #...######
// .##.#....#
// .###.#####
// ###.##.##.
// .###....#.
// ..#.#..#.#
// #...##.#..
// 
// Tile 1171:
// ####...##.
// #..##.#..#
// ##.#..#.#.
// .###.####.
// ..###.####
// .##....##.
// .#...####.
// #.##.####.
// ####..#...
// .....##...
// 
// Tile 1427:
// ###.##.#..
// .#..#.##..
// .#.##.#..#
// #.#.#.##.#
// ....#...##
// ...##..##.
// ...#.#####
// .#.####.#.
// ..#..###.#
// ..##.#..#.
// 
// Tile 1489:
// ##.#.#....
// ..##...#..
// .##..##...
// ..#...#...
// #####...#.
// #..#.#.#.#
// ...#.#.#..
// ##.#...##.
// ..##.##.##
// ###.##.#..
// 
// Tile 2473:
// #....####.
// #..#.##...
// #.##..#...
// ######.#.#
// .#...#.#.#
// .#########
// .###.#..#.
// ########.#
// ##...##.#.
// ..###.#.#.
// 
// Tile 2971:
// ..#.#....#
// #...###...
// #.#.###...
// ##.##..#..
// .#####..##
// .#..####.#
// #..#.#..#.
// ..####.###
// ..#.#.###.
// ...#.#.#.#
// 
// Tile 2729:
// ...#.#.#.#
// ####.#....
// ..#.#.....
// ....#..#.#
// .##..##.#.
// .#.####...
// ####.#.#..
// ##.####...
// ##..#.##..
// #.##...##.
// 
// Tile 3079:
// #.#.#####.
// .#..######
// ..#.......
// ######....
// ####.#..#.
// .#...#.##.
// #.#####.##
// ..#.###...
// ..#.......
// ..#.###...
// 
// By rotating, flipping, and rearranging them, you can find a square
// arrangement that causes all adjacent borders to line up:
// 
// #...##.#.. ..###..### #.#.#####.
// ..#.#..#.# ###...#.#. .#..######
// .###....#. ..#....#.. ..#.......
// ###.##.##. .#.#.#..## ######....
// .###.##### ##...#.### ####.#..#.
// .##.#....# ##.##.###. .#...#.##.
// #...###### ####.#...# #.#####.##
// .....#..## #...##..#. ..#.###...
// #.####...# ##..#..... ..#.......
// #.##...##. ..##.#..#. ..#.###...
// 
// #.##...##. ..##.#..#. ..#.###...
// ##..#.##.. ..#..###.# ##.##....#
// ##.####... .#.####.#. ..#.###..#
// ####.#.#.. ...#.##### ###.#..###
// .#.####... ...##..##. .######.##
// .##..##.#. ....#...## #.#.#.#...
// ....#..#.# #.#.#.##.# #.###.###.
// ..#.#..... .#.##.#..# #.###.##..
// ####.#.... .#..#.##.. .######...
// ...#.#.#.# ###.##.#.. .##...####
// 
// ...#.#.#.# ###.##.#.. .##...####
// ..#.#.###. ..##.##.## #..#.##..#
// ..####.### ##.#...##. .#.#..#.##
// #..#.#..#. ...#.#.#.. .####.###.
// .#..####.# #..#.#.#.# ####.###..
// .#####..## #####...#. .##....##.
// ##.##..#.. ..#...#... .####...#.
// #.#.###... .##..##... .####.##.#
// #...###... ..##...#.. ...#..####
// ..#.#....# ##.#.#.... ...##.....
// 
// For reference, the IDs of the above tiles are:
// 
// 1951       2311       3079   2729    1427    2473   2971       1489       1171   
// 
// To check that you've assembled the image correctly, multiply the IDs
// of the four corner tiles together. If you do this with the assembled
// tiles from the example above, you get 1951 * 3079 * 2971 * 1171 =
// 20899048083289.
// 
// Assemble the tiles into an image. What do you get if you multiply
// together the IDs of the four corner tiles?
//
// --- Part Two ---
// ----------------
// 
// Now, you're ready to check the image for sea monsters.
// 
// The borders of each tile are not part of the actual image; start by
// removing them.
// 
// In the example above, the tiles become:
// 
// .#.#..#. ##...#.# #..#####
// ###....# .#....#. .#......
// ##.##.## #.#.#..# #####...
// ###.#### #...#.## ###.#..#
// ##.#.... #.##.### #...#.##
// ...##### ###.#... .#####.#
// ....#..# ...##..# .#.###..
// .####... #..#.... .#......
// 
// #..#.##. .#..###. #.##....
// #.####.. #.####.# .#.###..
// ###.#.#. ..#.#### ##.#..##
// #.####.. ..##..## ######.#
// ##..##.# ...#...# .#.#.#..
// ...#..#. .#.#.##. .###.###
// .#.#.... #.##.#.. .###.##.
// ###.#... #..#.##. ######..
// 
// .#.#.### .##.##.# ..#.##..
// .####.## #.#...## #.#..#.#
// ..#.#..# ..#.#.#. ####.###
// #..####. ..#.#.#. ###.###.
// #####..# ####...# ##....##
// #.##..#. .#...#.. ####...#
// .#.###.. ##..##.. ####.##.
// ...###.. .##...#. ..#..###
// 
// Remove the gaps to form the actual image:
// 
// .#.#..#.##...#.##..#####
// ###....#.#....#..#......
// ##.##.###.#.#..######...
// ###.#####...#.#####.#..#
// ##.#....#.##.####...#.##
// ...########.#....#####.#
// ....#..#...##..#.#.###..
// .####...#..#.....#......
// #..#.##..#..###.#.##....
// #.####..#.####.#.#.###..
// ###.#.#...#.######.#..##
// #.####....##..########.#
// ##..##.#...#...#.#.#.#..
// ...#..#..#.#.##..###.###
// .#.#....#.##.#...###.##.
// ###.#...#..#.##.######..
// .#.#.###.##.##.#..#.##..
// .####.###.#...###.#..#.#
// ..#.#..#..#.#.#.####.###
// #..####...#.#.#.###.###.
// #####..#####...###....##
// #.##..#..#...#..####...#
// .#.###..##..##..####.##.
// ...###...##...#...#..###
// 
// Now, you're ready to search for sea monsters! Because your image is
// monochrome, a sea monster will look like this:
// 
// # 
// #    ##    ##    ###
// #  #  #  #  #  #   
// 
// When looking for this pattern in the image, the spaces can be anything;
// only the # need to match. Also, you might need to rotate or flip your
// image before it's oriented correctly to find sea monsters. In the
// above image, after flipping and rotating it to the appropriate
// orientation, there are two sea monsters (marked with O):
// 
// .####...#####..#...###..
// #####..#..#.#.####..#.#.
// .#.#...#.###...#.##.   O   #..
// #.   O   .##.   O   O   #.#.   O   O   .##.   O   O   O   ##
// ..#   O   .#   O   #.   O   ##   O   ..   O   .#   O   ##.##
// ...#.#..##.##...#..#..##
// #.##.#..#.#..#..##.#.#..
// .###.##.....#...###.#...
// #.####.#.#....##.#..#.#.
// ##...#..#....#..#...####
// ..#.##...###..#.#####..#
// ....#.##.#.#####....#...
// ..##.##.###.....#.##..#.
// #...#...###..####....##.
// .#.##...#.##.#.#.###...#
// #.###.#..####...##..#...
// #.###...#.##...#.##   O   ###.
// .   O   ##.#   O   O   .###   O   O   ##..   O   O   O   ##.
// ..   O   #.   O   ..   O   ..   O   .#   O   ##   O   ##.###
// #.#..##.########..#..##.
// #.#####..#.#...##..#....
// #....##..#.#########..##
// #...#.....#..##...###.##
// #..###....##.#...##.##.#
// 
// Determine how rough the waters are in the sea monsters' habitat by
// counting the number of # that are not part of a sea monster. In the
// above example, the habitat's water roughness is 273.
// 
// How many # are not part of a sea monster?


#include "advent_of_code/2020/day20/day20.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

std::string EdgeString(const CharBoard& board, Point start, Point end,
                       Point dir) {
  std::string row;
  row.resize((end - start).dist() + 1);
  for (Point p = start; true; p += dir) {
    row[(p - start).dist()] = board[p];
    if (p == end) break;
  }
  return row;
}

void AddEdge(const CharBoard& board, int tile_num, Point start, Point end,
             Point dir,
             absl::flat_hash_map<std::string, std::vector<int>>* edge_to_tile) {
  std::string row = EdgeString(board, start, end, dir);
  (*edge_to_tile)[row].push_back(tile_num);
  VLOG(3) << "Add edge: " << row << " to " << tile_num;
  row = EdgeString(board, end, start, -dir);
  (*edge_to_tile)[row].push_back(tile_num);
  VLOG(3) << "Add edge: " << row << " to " << tile_num;
}

absl::StatusOr<int64_t> AlignTileCorners(
    const absl::flat_hash_map<int, CharBoard>& tiles) {
  absl::flat_hash_map<std::string, std::vector<int>> edge_to_tile;
  for (const auto& [num, tile] : tiles) {
    PointRectangle range = tile.range();
    AddEdge(tile, num, range.min, {range.max.x, range.min.y}, {1, 0},
            &edge_to_tile);
    AddEdge(tile, num, range.min, {range.min.x, range.max.y}, {0, 1},
            &edge_to_tile);
    AddEdge(tile, num, {range.min.x, range.max.y}, range.max, {1, 0},
            &edge_to_tile);
    AddEdge(tile, num, {range.max.x, range.min.y}, range.max, {0, 1},
            &edge_to_tile);
  }
  absl::flat_hash_map<int, int> tile_to_single_edge;
  for (const auto& [edge, tile_ids] : edge_to_tile) {
    VLOG(3) << "edge-to-tiles: " << edge << " " << absl::StrJoin(tile_ids, ",");
    int delta = tile_ids.size() == 1 ? 1 : 0;
    for (int tile : tile_ids) {
      tile_to_single_edge[tile] += delta;
    }
  }
  int64_t ret = 1;
  int corners = 0;
  for (const auto& [tile, edge_count] : tile_to_single_edge) {
    // Edges are all in twice (for each orientation).
    VLOG(1) << "tile-to-edge: " << tile << " " << edge_count;
    if (edge_count == 4) {
      ret *= tile;
      ++corners;
    } else if (edge_count == 2) {
      // Edge.
    } else if (edge_count == 0) {
      // Middle.
    } else {
      return Error("Bad edge count");
    }
  }
  if (corners != 4) {
    return Error("Not 4 corners: ", corners);
  }
  return ret;
}

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

class TileMerger {
 public:
  TileMerger(const absl::flat_hash_map<int, CharBoard>& tiles)
      : tiles_(tiles) {}

  absl::StatusOr<CharBoard> Merge() {
    if (absl::Status st = InitSideLength(); !st.ok()) return st;
    if (absl::Status st = BuildEdgeMap(); !st.ok()) return st;
    if (absl::Status st = ClassifyTiles(); !st.ok()) return st;
    int merged_side_length = tile_edge_size_ * side_tile_count_;
    CharBoard merged(merged_side_length, merged_side_length);
    for (Point p : merged.range()) {
      merged[p] = '?';
    }
    for (int tile_y = 0; tile_y < side_tile_count_; ++tile_y) {
      for (int tile_x = 0; tile_x < side_tile_count_; ++tile_x) {
        if (absl::Status st = FindAndInsertTile(&merged, tile_x, tile_y);
            !st.ok())
          return st;
        VLOG(2) << "Merging board\n" << merged;
      }
    }
    VLOG(1) << "Board with boarders:\n" << merged;
    CharBoard no_borders = RemoveBorder(merged);
    VLOG(1) << "Board without boarders:\n" << no_borders;
    return no_borders;
  }

 private:
  absl::Status InitSideLength() {
    side_tile_count_ = 1;
    for (; side_tile_count_ * side_tile_count_ < tiles_.size();
         ++side_tile_count_) {
    }
    if (side_tile_count_ * side_tile_count_ != tiles_.size()) {
      return Error("Tiles aren't square-able: ", tiles_.size());
    }
    VLOG(1) << "Final board is " << side_tile_count_ << "x" << side_tile_count_
            << " tiles";
    return absl::OkStatus();
  }

  absl::Status BuildEdgeMap() {
    tile_edge_size_ = tiles_.begin()->second.width();
    if (tile_edge_size_ != tiles_.begin()->second.height()) {
      return Error("Tile isn't square");
    }
    for (const auto& [num, tile] : tiles_) {
      if (tile.width() != tile_edge_size_) {
        return Error("Widths aren't consistent");
      }
      if (tile.height() != tile_edge_size_) {
        return Error("Heights aren't consistent");
      }
      PointRectangle range = tile.range();
      AddEdge(tile, num, range.min, {range.max.x, range.min.y}, {1, 0},
              &edge_to_tile_);
      AddEdge(tile, num, range.min, {range.min.x, range.max.y}, {0, 1},
              &edge_to_tile_);
      AddEdge(tile, num, {range.min.x, range.max.y}, range.max, {1, 0},
              &edge_to_tile_);
      AddEdge(tile, num, {range.max.x, range.min.y}, range.max, {0, 1},
              &edge_to_tile_);
    }
    return absl::OkStatus();
  }

  absl::Status ClassifyTiles() {
    absl::flat_hash_map<int, int> tile_to_single_edge;
    for (const auto& [edge, tile_ids] : edge_to_tile_) {
      VLOG(3) << "edge-to-tiles: " << edge << " "
              << absl::StrJoin(tile_ids, ",");
      int delta = tile_ids.size() == 1 ? 1 : 0;
      for (int tile : tile_ids) {
        tile_to_single_edge[tile] += delta;
      }
    }
    for (const auto& [tile, edge_count] : tile_to_single_edge) {
      // Edges are all in twice (for each orientation).
      if (edge_count == 4) {
        corners_.insert(tile);
      } else if (edge_count == 2) {
        edges_.insert(tile);
      } else if (edge_count == 0) {
        middles_.insert(tile);
      } else {
        return Error("Bad edge count");
      }
    }
    return absl::OkStatus();
  }

  absl::Status FindAndInsertTile(CharBoard* board, int tile_x, int tile_y) {
    Point fill_from = {tile_x * tile_edge_size_, tile_y * tile_edge_size_};
    absl::StatusOr<CharBoard> oriented =
        absl::NotFoundError("Nothing looked up");

    std::string orient_x;
    if (tile_y > 0) {
      orient_x.resize(tile_edge_size_);
      Point orient_x_start = {tile_x * tile_edge_size_,
                              tile_y * tile_edge_size_ - 1};
      Point orient_x_end = {(tile_x + 1) * tile_edge_size_,
                            tile_y * tile_edge_size_ - 1};
      for (Point p = orient_x_start; p != orient_x_end; p += {1, 0}) {
        orient_x[(p - orient_x_start).dist()] = (*board)[p];
      }
    }
    std::string orient_y;
    if (tile_x > 0) {
      orient_y.resize(tile_edge_size_);
      Point orient_y_start = {tile_x * tile_edge_size_ - 1,
                              tile_y * tile_edge_size_};
      Point orient_y_end = {tile_x * tile_edge_size_ - 1,
                            (tile_y + 1) * tile_edge_size_};
      for (Point p = orient_y_start; p != orient_y_end; p += {0, 1}) {
        orient_y[(p - orient_y_start).dist()] = (*board)[p];
      }
    }

    oriented = Orient(orient_x, orient_y);
    if (!oriented.ok()) return oriented.status();

    for (Point p : oriented->range()) {
      (*board)[fill_from + p] = (*oriented)[p];
    }
    return absl::OkStatus();
  }

  absl::StatusOr<CharBoard> Orient(std::string x_align, std::string y_align) {
    VLOG(3) << "Orienting";
    VLOG(3) << "  x_align: " << x_align;
    VLOG(3) << "  y_align: " << y_align;
    absl::optional<int> tile_num;
    if (x_align.empty() && y_align.empty()) {
      // First corner. Pick arbitrarily.
      if (corners_.empty()) return Error("No corners?");
      tile_num = *corners_.begin();
    } else if (y_align.empty()) {
      auto x_it = edge_to_tile_.find(x_align);
      if (x_it == edge_to_tile_.end()) return Error("Can't find x: ", x_align);
      absl::flat_hash_set<int> in_y;
      for (int y_tile : corners_) {
        in_y.insert(y_tile);
      }
      for (int y_tile : edges_) {
        in_y.insert(y_tile);
      }
      for (int x_tile : x_it->second) {
        if (in_y.contains(x_tile) && !used_tiles_.contains(x_tile)) {
          if (tile_num) return Error("Tile not unique");
          tile_num = x_tile;
        }
      }
    } else if (x_align.empty()) {
      auto y_it = edge_to_tile_.find(y_align);
      if (y_it == edge_to_tile_.end()) return Error("Can't find y: ", y_align);
      absl::flat_hash_set<int> in_x;
      for (int x_tile : corners_) {
        in_x.insert(x_tile);
      }
      for (int x_tile : edges_) {
        in_x.insert(x_tile);
      }
      for (int y_tile : y_it->second) {
        if (in_x.contains(y_tile) && !used_tiles_.contains(y_tile)) {
          if (tile_num) return Error("Tile not unique");
          tile_num = y_tile;
        }
      }
    } else {
      auto x_it = edge_to_tile_.find(x_align);
      if (x_it == edge_to_tile_.end()) return Error("Can't find x");
      auto y_it = edge_to_tile_.find(y_align);
      if (y_it == edge_to_tile_.end()) return Error("Can't find y");
      absl::flat_hash_set<int> in_x;
      for (int x_tile : x_it->second) in_x.insert(x_tile);
      for (int y_tile : y_it->second) {
        if (in_x.contains(y_tile) && !used_tiles_.contains(y_tile)) {
          if (tile_num) return Error("Tile not unique");
          tile_num = y_tile;
        }
      }
    }

    if (!tile_num) {
      return Error("Not supported");
    }
    auto tile_it = tiles_.find(*tile_num);
    used_tiles_.insert(*tile_num);
    if (tile_it == tiles_.end()) return Error("Can't find tile: ", *tile_num);
    const CharBoard& tile = tile_it->second;

    VLOG(3) << "  tile\n" << tile;
    for (std::function<Point(Point)> t : Transforms(tile.range())) {
      CharBoard tmp = Transform(tile, t);
      VLOG(4) << "Oriented to\n" << tmp;

      std::string x_edge =
          EdgeString(tmp, {0, 0}, {tmp.width() - 1, 0}, {1, 0});
      std::string y_edge =
          EdgeString(tmp, {0, 0}, {0, tmp.height() - 1}, {0, 1});

      VLOG(4) << "  x_edge: " << x_edge;
      VLOG(4) << "  y_edge: " << y_edge;

      if (x_align.empty() && edge_to_tile_[x_edge].size() != 1) continue;
      if (!x_align.empty() && x_edge != x_align) continue;
      if (y_align.empty() && edge_to_tile_[y_edge].size() != 1) continue;
      if (!y_align.empty() && y_edge != y_align) continue;

      return tmp;
    }

    return Error("Could not orient board");
  }

  CharBoard RemoveBorder(const CharBoard& in) {
    CharBoard no_borders(in.width() - 2 * side_tile_count_,
                         in.height() - 2 * side_tile_count_);
    Point out = {0, 0};
    for (int y = 0; y < in.height(); ++y) {
      if (y % tile_edge_size_ == 0) continue;
      if (y % tile_edge_size_ == tile_edge_size_ - 1) continue;
      for (int x = 0; x < in.width(); ++x) {
        if (x % tile_edge_size_ == 0) continue;
        if (x % tile_edge_size_ == tile_edge_size_ - 1) continue;
        no_borders[out] = in[Point{x, y}];
        ++out.x;
      }
      out.x = 0;
      ++out.y;
    }
    return no_borders;
  }

  int side_tile_count_;
  int tile_edge_size_;
  const absl::flat_hash_map<int, CharBoard>& tiles_;
  absl::flat_hash_map<std::string, std::vector<int>> edge_to_tile_;
  absl::flat_hash_set<int> corners_;
  absl::flat_hash_set<int> edges_;
  absl::flat_hash_set<int> middles_;
  absl::flat_hash_set<int> used_tiles_;
};

bool IsSeaMonster(const CharBoard& board, Point p) {
  //                  # ",
  //#    ##    ##    ###",
  // #  #  #  #  #  #   ",
  absl::flat_hash_set<Point> kSeaMonster = {
      {0, 1},  {1, 2},  {4, 2},  {5, 1},  {6, 1},  {7, 2},  {10, 2}, {11, 1},
      {12, 1}, {13, 2}, {16, 2}, {17, 1}, {18, 1}, {18, 0}, {19, 1},
  };

  for (Point check : kSeaMonster) {
    if (!board.OnBoard(p + check)) return false;
    if (board[p + check] != '#') return false;
  }
  return true;
}

absl::StatusOr<int> CountNonSeaMonster(const CharBoard& board) {
  const int kSeaMonsterOn = 15;
  int on = board.CountOn();
  absl::optional<int> sea_monster_count;
  for (std::function<Point(Point)> t : Transforms(board.range())) {
    CharBoard tmp = Transform(board, t);
    LOG(INFO) << "Looking for monsters in\n" << tmp;
    int this_sea_monster_count = 0;
    for (Point p : tmp.range()) {
      if (IsSeaMonster(tmp, p)) {
        LOG(INFO) << "Found at: " << p;
        ++this_sea_monster_count;
      }
    }
    if (sea_monster_count && this_sea_monster_count > 0) {
      return Error("Multiple sea monster orientations");
    } else if (this_sea_monster_count > 0) {
      sea_monster_count = this_sea_monster_count;
    }
  }
  if (!sea_monster_count) return Error("No monsters found");
  return on - *sea_monster_count * kSeaMonsterOn;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2020_20::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<int, CharBoard> tiles;
  int cur_tile_num;
  int cur_tile_index;
  for (int i = 0; i < input.size(); ++i) {
    if (RE2::FullMatch(input[i], "Tile (\\d+):", &cur_tile_num)) {
      cur_tile_index = i + 1;
    } else if (input[i].empty()) {
      absl::StatusOr<CharBoard> board =
          ParseAsBoard(input.subspan(cur_tile_index, i - cur_tile_index));
      if (!board.ok()) return board.status();
      if (tiles.contains(cur_tile_num)) return Error("Dup tile:", cur_tile_num);
      tiles.emplace(cur_tile_num, std::move(*board));
    }
  }
  absl::StatusOr<CharBoard> board =
      ParseAsBoard(input.subspan(cur_tile_index, input.size()));
  if (!board.ok()) return board.status();
  if (tiles.contains(cur_tile_num)) return Error("Dup tile:", cur_tile_num);
  tiles.emplace(cur_tile_num, std::move(*board));

  return IntReturn(AlignTileCorners(tiles));
}

absl::StatusOr<std::string> Day_2020_20::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<int, CharBoard> tiles;
  int cur_tile_num;
  int cur_tile_index;
  for (int i = 0; i < input.size(); ++i) {
    if (RE2::FullMatch(input[i], "Tile (\\d+):", &cur_tile_num)) {
      cur_tile_index = i + 1;
    } else if (input[i].empty()) {
      absl::StatusOr<CharBoard> board =
          ParseAsBoard(input.subspan(cur_tile_index, i - cur_tile_index));
      if (!board.ok()) return board.status();
      if (tiles.contains(cur_tile_num)) return Error("Dup tile:", cur_tile_num);
      tiles.emplace(cur_tile_num, std::move(*board));
    }
  }
  absl::StatusOr<CharBoard> board =
      ParseAsBoard(input.subspan(cur_tile_index, input.size()));
  if (!board.ok()) return board.status();
  if (tiles.contains(cur_tile_num)) return Error("Dup tile:", cur_tile_num);
  tiles.emplace(cur_tile_num, std::move(*board));

  absl::StatusOr<CharBoard> merged = TileMerger(tiles).Merge();
  if (!merged.ok()) return merged.status();

  VLOG(1) << "Merged Board:\n" << *merged;

  return IntReturn(CountNonSeaMonster(*merged));
}

}  // namespace advent_of_code
