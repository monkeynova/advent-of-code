#include "advent_of_code/2020/day20/day20.h"

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

void AddEdge(const CharBoard& board, int tile_num, Point start, Point end, Point dir,
            absl::flat_hash_map<std::string, std::vector<int>>* edge_to_tile) {
  std::string row;
  row.resize((end - start).dist() + 1);
  for (Point p = start; true; p += dir) {
    row[(p - start).dist()] = board[p];
    if (p == end) break;
  }
  (*edge_to_tile)[row].push_back(tile_num);
  VLOG(2) << "Add edge: " << row << " to " << tile_num;
  for (Point p = end; true; p -= dir) {
    row[(p - end).dist()] = board[p];
    if (p == start) break;
  }
  (*edge_to_tile)[row].push_back(tile_num);
  VLOG(2) << "Add edge: " << row << " to " << tile_num;
}

absl::StatusOr<int64_t> AlignTileCorners(const absl::flat_hash_map<int, CharBoard>& tiles) {
  absl::flat_hash_map<std::string, std::vector<int>> edge_to_tile;
  for (const auto& [num, tile] : tiles) {
    PointRectangle range = tile.range();
    AddEdge(tile, num, range.min, {range.max.x, range.min.y}, {1, 0}, &edge_to_tile);
    AddEdge(tile, num, range.min, {range.min.x, range.max.y}, {0, 1}, &edge_to_tile);
    AddEdge(tile, num, {range.min.x, range.max.y}, range.max, {1, 0}, &edge_to_tile);
    AddEdge(tile, num, {range.max.x, range.min.y}, range.max, {0, 1}, &edge_to_tile);
  }
  absl::flat_hash_map<int, int> tile_to_single_edge;
  for (const auto& [edge, tile_ids] : edge_to_tile) {
    VLOG(2) << "edge-to-tiles: " << edge << " " << absl::StrJoin(tile_ids, ",");
    if (tile_ids.size() == 1) {
      for (int tile : tile_ids) {
        ++tile_to_single_edge[tile];
      }
    }
  }
  int64_t ret = 1;
  int corners = 0;
  for (const auto& [tile, edge_count] : tile_to_single_edge) {
    // Edges are all in twice (for each orientation).
    VLOG(1) << "tile-to-edge: " << tile << " " << edge_count;
    if (edge_count != 2 and edge_count != 4) {
      return AdventDay::Error("Bad edge count");
    }
    if (edge_count == 4) {
      ret *= tile;
      ++corners;
    }
  }
  if (corners != 4) {
    return AdventDay::Error("Not 4 corners: ", corners);
  }
  return ret;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day20_2020::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<int, CharBoard> tiles;
  int cur_tile_num;
  int cur_tile_index;
  for (int i = 0; i < input.size(); ++i) {
    if (RE2::FullMatch(input[i], "Tile (\\d+):", &cur_tile_num)) {
      cur_tile_index = i + 1;
    } else if (input[i].empty()) {
      absl::StatusOr<CharBoard> board = CharBoard::Parse(input.subspan(cur_tile_index, i - cur_tile_index));
      if (!board.ok()) return board.status();
      if (tiles.contains(cur_tile_num)) return Error("Dup tile:", cur_tile_num);
      tiles.emplace(cur_tile_num, std::move(*board));
    }
  }
  absl::StatusOr<CharBoard> board = CharBoard::Parse(input.subspan(cur_tile_index, input.size()));
  if (!board.ok()) return board.status();
  if (tiles.contains(cur_tile_num)) return Error("Dup tile:", cur_tile_num);
  tiles.emplace(cur_tile_num, std::move(*board));

  return IntReturn(AlignTileCorners(tiles));
}

absl::StatusOr<std::vector<std::string>> Day20_2020::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
