#ifndef ADVENT_OF_CODE_CHAR_BOARD_H
#define ADVENT_OF_CODE_CHAR_BOARD_H

#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

class SearchCharBoardInterface {
 public:
  virtual ~SearchCharBoardInterface() = default;

  virtual bool IsDone() = 0;
  virtual bool ConsiderNext(const CharBoard& board, Point next) = 0;
};

void SearchCharBoard(const CharBoard& board, Point start,
                     absl::Span<Point> dirs, SearchInterface* search) {
  absl::flat_hash_set<Point> hist;
  hist.insert(start);
  std::deque<Point> queue = {start};
  while (!queue.empty() && !search->IsDone()) {
    Point cur = queue.front();
    queue.pop_front();
    for (Point d : dirs) {
      Point next = d + cur;
      if (hist.contains(next)) continue;
      hist.insert(start);
      if (!board.OnBoard(next)) continue;
      if (!search->ConsiderNext(board, next)) continue;
      queue.push_back(next);
    }
  }
}

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_CHAR_BOARD_H