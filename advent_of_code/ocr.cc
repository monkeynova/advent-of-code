#include "advent_of_code/ocr.h"

#include "absl/algorithm/container.h"

namespace advent_of_code {

namespace {

std::vector<std::pair<char, CharBoard>> Exemplars() {
  std::vector<std::pair<char, CharBoard>> exemplars;
  exemplars.emplace_back('A', *CharBoard::Parse(std::vector<std::string>{
                                  ".###.",
                                  "#...#",
                                  "#####",
                                  "#...#",
                                  "#...#",
                              }));
  exemplars.emplace_back('B', *CharBoard::Parse(std::vector<std::string>{
                                  "####.",
                                  "#...#",
                                  "####.",
                                  "#...#",
                                  "####.",
                              }));
  exemplars.emplace_back('C', *CharBoard::Parse(std::vector<std::string>{
                                  ".###.",
                                  "#...#",
                                  "#....",
                                  "#...#",
                                  ".###.",
                              }));
  exemplars.emplace_back('D', *CharBoard::Parse(std::vector<std::string>{
                                  "####.",
                                  "#...#",
                                  "#...#",
                                  "#...#",
                                  "####.",
                              }));
  exemplars.emplace_back('E', *CharBoard::Parse(std::vector<std::string>{
                                  "#####",
                                  "#....",
                                  "####.",
                                  "#....",
                                  "#####",
                              }));
  exemplars.emplace_back('F', *CharBoard::Parse(std::vector<std::string>{
                                  "#####",
                                  "#....",
                                  "####.",
                                  "#....",
                                  "#....",
                              }));
  exemplars.emplace_back('G', *CharBoard::Parse(std::vector<std::string>{
                                  ".###.",
                                  "#....",
                                  "#..##",
                                  "#...#",
                                  ".####",
                              }));
  exemplars.emplace_back('H', *CharBoard::Parse(std::vector<std::string>{
                                  "#...#",
                                  "#...#",
                                  "#####",
                                  "#...#",
                                  "#...#",
                              }));
  exemplars.emplace_back('I', *CharBoard::Parse(std::vector<std::string>{
                                  "#####",
                                  "..#..",
                                  "..#..",
                                  "..#..",
                                  "#####",
                              }));
  exemplars.emplace_back('J', *CharBoard::Parse(std::vector<std::string>{
                                  "...##",
                                  "....#",
                                  "....#",
                                  "#...#",
                                  ".####",
                              }));
  exemplars.emplace_back('K', *CharBoard::Parse(std::vector<std::string>{
                                  "#..#",
                                  "#.#.",
                                  "##..",
                                  "#.#.",
                                  "#..#",
                              }));
  exemplars.emplace_back('L', *CharBoard::Parse(std::vector<std::string>{
                                  "#....",
                                  "#....",
                                  "#....",
                                  "#....",
                                  "#####",
                              }));
  exemplars.emplace_back('M', *CharBoard::Parse(std::vector<std::string>{
                                  "#...#",
                                  "##.##",
                                  "#.#.#",
                                  "#...#",
                                  "#...#",
                              }));
  exemplars.emplace_back('N', *CharBoard::Parse(std::vector<std::string>{
                                  "#...#",
                                  "##..#",
                                  "#.#.#",
                                  "#..##",
                                  "#...#",
                              }));
  exemplars.emplace_back('O', *CharBoard::Parse(std::vector<std::string>{
                                  ".###.",
                                  "#...#",
                                  "#...#",
                                  "#...#",
                                  ".###.",
                              }));
  exemplars.emplace_back('O', *CharBoard::Parse(std::vector<std::string>{
                                  "#####",
                                  "#...#",
                                  "#...#",
                                  "#...#",
                                  "#####",
                              }));
  exemplars.emplace_back('P', *CharBoard::Parse(std::vector<std::string>{
                                  "####.",
                                  "#...#",
                                  "####.",
                                  "#....",
                                  "#....",
                              }));
#if 0
  exemplars.emplace_back('Q', *CharBoard::Parse(std::vector<std::string>{
    ".###.",
    "#...#",
    "#...#",
    "#..##",
    ".####",
  }));
#endif
  exemplars.emplace_back('R', *CharBoard::Parse(std::vector<std::string>{
                                  "####.",
                                  "#...#",
                                  "####.",
                                  "#..#.",
                                  "#...#",
                              }));
  exemplars.emplace_back('S', *CharBoard::Parse(std::vector<std::string>{
                                  ".####",
                                  "#....",
                                  " ###.",
                                  "....#",
                                  "####.",
                              }));
  exemplars.emplace_back('T', *CharBoard::Parse(std::vector<std::string>{
                                  "#####",
                                  "..#..",
                                  "..#..",
                                  "..#..",
                                  "..#..",
                              }));
  exemplars.emplace_back('U', *CharBoard::Parse(std::vector<std::string>{
                                  "#...#",
                                  "#...#",
                                  "#...#",
                                  "#...#",
                                  ".###.",
                              }));
  exemplars.emplace_back('V', *CharBoard::Parse(std::vector<std::string>{
                                  "#...#",
                                  "#...#",
                                  ".#.#.",
                                  ".#.#.",
                                  "..#..",
                              }));
  exemplars.emplace_back('W', *CharBoard::Parse(std::vector<std::string>{
                                  "#...#",
                                  "#...#",
                                  "#.#.#",
                                  "##.##",
                                  "#...#",
                              }));
  exemplars.emplace_back('X', *CharBoard::Parse(std::vector<std::string>{
                                  "#...#",
                                  ".#.#.",
                                  "..#..",
                                  ".#.#.",
                                  "#...#",
                              }));
  exemplars.emplace_back('Y', *CharBoard::Parse(std::vector<std::string>{
                                  "#...#",
                                  ".#.#.",
                                  "..#..",
                                  "..#..",
                                  "..#..",
                              }));
  exemplars.emplace_back('Z', *CharBoard::Parse(std::vector<std::string>{
                                  "#####",
                                  "...#.",
                                  "..#..",
                                  ".#...",
                                  "#####",
                              }));
  return exemplars;
}

absl::StatusOr<CharBoard> ExtractNextChar(int start_col, const CharBoard& board,
                                          int* end_col) {
  for (*end_col = start_col + 1; *end_col < board.width(); ++*end_col) {
    bool empty_col = true;
    for (Point test = {*end_col, 0}; test.y < board.height(); ++test.y) {
      if (board[test] != '.') {
        empty_col = false;
        break;
      }
    }
    if (empty_col) {
      break;
    }
  }
  return board.SubBoard(
      PointRectangle{{start_col, 0}, {*end_col - 1, board.height() - 1}});
}

double Score(const CharBoard& test, const CharBoard& exemplar) {
  double overlap_score = 0;
  double dy = 0.01;
  double dx = 0.01;
  for (double x = 0; x < 1; x += dx) {
    for (double y = 0; y < 1; y += dy) {
      Point test_p{static_cast<int>(x * test.width()),
                   static_cast<int>(y * test.height())};
      Point exemplar_p{static_cast<int>(x * exemplar.width()),
                       static_cast<int>(y * exemplar.height())};
      if (test[test_p] == exemplar[exemplar_p]) {
        overlap_score += dx * dy;
      }
    }
  }
  return overlap_score;
}

absl::StatusOr<char> OCRChar(
    const CharBoard& board,
    const std::vector<std::pair<char, CharBoard>>& exemplars) {
  VLOG(1) << board.AsString();
  char best_c;
  double best_score = -1;
  for (const auto& [e_char, e_board] : exemplars) {
    double score = Score(board, e_board);
    VLOG(1) << absl::string_view(&e_char, 1) << " -> " << score;
    if (score > best_score) {
      best_score = score;
      best_c = e_char;
    }
  }
  return best_c;
}

}  // namespace

absl::StatusOr<std::string> OCRExtract(const CharBoard& board) {
  std::vector<std::pair<char, CharBoard>> exemplars = Exemplars();

  std::string ret;
  for (int char_start = 0, char_end; char_start < board.width();
       char_start = char_end + 1) {
    absl::StatusOr<CharBoard> single_char =
        ExtractNextChar(char_start, board, &char_end);
    if (!single_char.ok()) return single_char.status();

    absl::StatusOr<char> next_char = OCRChar(*single_char, exemplars);
    if (!next_char.ok()) return next_char.status();

    ret.append(1, *next_char);
  }
  return absl::StrCat(board.AsString(), "\n", ret);
}

}  // namespace advent_of_code
