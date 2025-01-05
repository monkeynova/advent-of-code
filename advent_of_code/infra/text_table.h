#ifndef ADVENT_OF_CODE_INFRA_TEXT_TABLE_H
#define ADVENT_OF_CODE_INFRA_TEXT_TABLE_H

#include <string>
#include <vector>

namespace advent_of_code {

class Table {
 public:
  struct Cell {
    enum Justify { kLeft = 0, kRight = 1, kCenter = 2 };
    enum Color { kWhite = 0, kYellow = 1, kGreen = 2, kRed = 3 };
    std::string entry;
    Justify justify = kLeft;
    Color color = kWhite;
    bool bold = false;
    int span = 1;

    void Render(std::string* out, int width, bool enable_color) const;
  };

  Table();

  void AddBreaker() { rows_.emplace_back(Breaker()); }

  void AddRow(std::vector<Cell> row) { rows_.emplace_back(std::move(row)); }

  std::string Render() const;

 private:
  std::vector<int> Layout() const;
  void RenderBreaker(std::string* out, const std::vector<int> col_widths,
                     int row_num) const;

  struct Breaker {};
  using Row = std::variant<Breaker, std::vector<Cell>>;
  bool use_color_ = false;
  std::vector<Row> rows_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_INFRA_TEXT_TABLE_H