#include "absl/container/flat_hash_set.h"
#include "absl/flags/flag.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/advent_day.h"
#include "advent_of_code/infra/file_util.h"
#include "advent_of_code/vlog.h"
#include "benchmark/benchmark.h"
#include "main_lib.h"

ABSL_FLAG(int, year, 2023, "Year to run");

namespace {

struct Input {
  Input() = default;

  // Neigher copyable nor movable. `lines` refers into `file`.
  Input(const Input&) = delete;
  Input(Input&&) = delete;

  std::string file;
  std::vector<std::string_view> lines;
};

absl::Status ReadInput(advent_of_code::AdventDay* day, Input* ret) {
  if (day == nullptr) return absl::InvalidArgumentError("null day");
  std::string filename(day->test_file());
  filename.erase(filename.rfind('/'));
  filename.append("/input.txt");
  ASSIGN_OR_RETURN(ret->file, advent_of_code::GetContents(filename));
  ret->lines = absl::StrSplit(ret->file, '\n');
  while (!ret->lines.empty() && ret->lines.back().empty()) {
    ret->lines.pop_back();
  }
  return absl::OkStatus();  
}

struct DayRun {
  absl::Duration time;
  std::string title;
  std::string part1;
  std::string part2;
};

absl::StatusOr<DayRun> RunDay(advent_of_code::AdventDay* day) {
  if (day == nullptr) {
    return DayRun{.time = absl::Seconds(0), .title = "???", .part1 = "", .part2 = ""};
  }
  DayRun ret;
  absl::Time start = absl::Now();
  ret.title = std::string(day->title());
  Input input;
  RETURN_IF_ERROR(ReadInput(day, &input));
  absl::StatusOr<std::string> part1 = day->Part1(absl::MakeSpan(input.lines));
  if (part1.ok()) ret.part1 = *std::move(part1);
  else ret.part1 = absl::StrCat("Error: ", part1.status().message());
  absl::StatusOr<std::string> part2 = day->Part2(absl::MakeSpan(input.lines));
  if (part2.ok()) ret.part2 = *std::move(part2);
  else ret.part2 = absl::StrCat("Error: ", part2.status().message());
  ret.time = absl::Now() - start;
  return ret;
}

class Table {
 public: 
  struct Cell {
    enum Justify { kLeft = 0, kRight = 1, kCenter = 2 };
    std::string entry;
    Justify justify = kLeft;
    int span = 1;
  };

  Table() = default;

  void AddBreaker() {
    rows_.emplace_back(Breaker());
  }

  void AddRow(std::vector<Cell> row) {
    rows_.emplace_back(std::move(row));
  }

  std::string Render() {
    std::string ret;
    
    std::vector<int> col_widths = Layout();

    std::string breaker = "+";
    for (int i = 0; i < col_widths.size(); ++i) {
      breaker.append(col_widths[i] + 2, '-');
      breaker.append(1, '+');
    }
    for (const auto& row : rows_) {
      if (std::holds_alternative<Breaker>(row)) {
        absl::StrAppend(&ret, breaker, "\n");
      } else if (std::holds_alternative<std::vector<Cell>>(row)) {
        ret.append(1, '|');
        const std::vector<Cell>& cells = std::get<std::vector<Cell>>(row);
        int col_idx = 0;
        for (const Cell& cell : cells) {
          int width = 0;
          for (int i = col_idx; i < col_idx + cell.span; ++i) {
            if (i > col_idx) width += 3; // Margin.
            width += col_widths[i];
          }
          ret.append(1, ' ');
          switch (cell.justify) {
            case Cell::kRight: {
              ret.append(width - cell.entry.size(), ' ');
              ret.append(cell.entry);
              break;
            }
            case Cell::kLeft: {
              ret.append(cell.entry);
              ret.append(width - cell.entry.size(), ' ');
              break;
            }
            case Cell::kCenter: {
              int left = (width - cell.entry.size()) / 2;
              int right = width - cell.entry.size() - left;
              ret.append(left, ' ');
              ret.append(cell.entry);
              ret.append(right, ' ');
              break;
            }
          }
          ret.append(" |");
          col_idx += cell.span;
        }
        ret.append("\n");
      }
    }

    return ret;
  }

 private:
  std::vector<int> Layout() const {
    absl::flat_hash_set<int> spans;
    int max_span = 0;
    for (const Row& row : rows_) {
      if (!std::holds_alternative<std::vector<Cell>>(row)) {
        continue;
      }
      const auto& cells = std::get<std::vector<Cell>>(row);
      int row_span = 0;
      for (const Cell& cell : cells) {
        spans.insert(cell.span);
        row_span += cell.span;
      }
      max_span = std::max(max_span, row_span);
    }

    std::vector<int> col_widths(max_span, 0);
    std::vector<int> spans_vec(spans.begin(), spans.end());
    absl::c_sort(spans_vec);

    for (int cur_span : spans) {
      for (const Row& row : rows_) {
        if (!std::holds_alternative<std::vector<Cell>>(row)) {
          continue;
        }
        const auto& cells = std::get<std::vector<Cell>>(row);
        int col_idx = 0;
        for (const Cell& cell : cells) {
          if (cell.span == cur_span) {
            int need = cell.entry.size();
            int have = 0;
            for (int i = col_idx; i < col_idx + cell.span; ++i) {
              if (i != col_idx) have += 3;  // Margin.
              have += col_widths[i];
            }
            if (need > have) {
              int delta = need - have;
              int add_per_col = delta / cell.span;
              int change_at = col_idx + cell.span;
              if (delta % cell.span != 0) {
                change_at = delta % cell.span;
                ++add_per_col;
              }
              for (int i = col_idx; i < col_idx + cell.span; ++i) {
                col_widths[i] += add_per_col;
                if (i == change_at) {
                  --add_per_col;
                }
              }
            }
          }
          col_idx += cell.span;
        }
      }
    }
    return col_widths;
  }

  struct Breaker {};
  using Row = std::variant<Breaker, std::vector<Cell>>;
  std::vector<Row> rows_;
};

}

int main(int argc, char** argv) {
  InitMain(argc, argv);
  int year = absl::GetFlag(FLAGS_year);
  std::vector<std::unique_ptr<advent_of_code::AdventDay>> days;
  for (int day = 1; day <= 25; ++day) {
    days.push_back(advent_of_code::CreateAdventDay(year, day));
  }
  std::vector<DayRun> runs;
  absl::Duration total_time = absl::Seconds(0);
  for (const auto& day : days) {
    absl::StatusOr<DayRun> run = RunDay(day.get());
    CHECK(run.ok()) << run.status().message() << day->test_file();
    runs.push_back(*std::move(run));
    total_time += runs.back().time;
  }

  Table table;
  table.AddBreaker();
  table.AddRow({
    Table::Cell{.entry = absl::StrCat("Advent of Code ", year), .span = 4,
                .justify = Table::Cell::kCenter}});
  table.AddBreaker();
  table.AddRow({
    Table::Cell{.entry = "Title"},
    Table::Cell{.entry = "Part 1"},
    Table::Cell{.entry = "Part 2"},
    Table::Cell{.entry = "Time"}});
  table.AddBreaker();
  for (const DayRun& run : runs) {
    table.AddRow({
      Table::Cell{.entry = run.title},
      Table::Cell{.entry = run.part1},
      Table::Cell{.entry = run.part2},
      Table::Cell{.entry = absl::StrCat(run.time),
                  .justify = Table::Cell::kRight}});
  }
  table.AddBreaker();
  table.AddRow({
    Table::Cell{.entry = "Total", .span = 3},
    Table::Cell{.entry = absl::StrCat(total_time),
                .justify = Table::Cell::kRight}});
  table.AddBreaker();

  std::cout << table.Render();
}
