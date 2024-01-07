#include "absl/container/flat_hash_set.h"
#include "absl/flags/flag.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/advent_day.h"
#include "advent_of_code/infra/file_util.h"
#include "benchmark/benchmark.h"
#include "main_lib.h"
#include "vlog.h"

enum ColorType {
  kAuto = 0,
  kNone = 1,
  kForce = 2,
};

ABSL_FLAG(int, year, 2023, "Year to run");
ABSL_FLAG(ColorType, color, ColorType::kAuto, "Color mode");

bool AbslParseFlag(absl::string_view text, ColorType* c, std::string* error) {
  if (text == "auto") {
    *c = kAuto;
    return true;
  }
  if (text == "none") {
    *c = kNone;
    return true;
  }
  if (text == "force") {
    *c = kForce;
    return true;
  }
  *error = "Color mode must be one of 'auto', 'none', or 'force'";
  return false;
}

std::string AbslUnparseFlag(ColorType c) {
  switch (c) {
    case kAuto:
      return "auto";
    case kNone:
      return "none";
    case kForce:
      return "force";
  }
  return "<Bad ColorType>";
}

namespace {

bool IsColorTerminal() {
  // This list of supported TERM values is copied from Google Test:
  // <https://github.com/google/googletest/blob/v1.13.0/googletest/src/gtest.cc#L3225-L3259>.
  const char* const SUPPORTED_TERM_VALUES[] = {
      "xterm",
      "xterm-color",
      "xterm-256color",
      "screen",
      "screen-256color",
      "tmux",
      "tmux-256color",
      "rxvt-unicode",
      "rxvt-unicode-256color",
      "linux",
      "cygwin",
      "xterm-kitty",
      "alacritty",
      "foot",
      "foot-extra",
      "wezterm",
  };

  const char* const term = getenv("TERM");

  bool term_supports_color = false;
  for (const char* candidate : SUPPORTED_TERM_VALUES) {
    if (term && 0 == strcmp(term, candidate)) {
      term_supports_color = true;
      break;
    }
  }

  return 0 != isatty(fileno(stdout)) && term_supports_color;
}

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
  bool part1_solved;
  std::string part2;
  bool part2_solved;
};

absl::StatusOr<DayRun> RunDay(advent_of_code::AdventDay* day) {
  if (day == nullptr) {
    return DayRun{.time = absl::Seconds(0),
                  .title = "???",
                  .part1 = "",
                  .part1_solved = false,
                  .part2 = "",
                  .part2_solved = false};
  }
  DayRun ret;
  absl::Time start = absl::Now();
  ret.title = std::string(day->title());
  Input input;
  RETURN_IF_ERROR(ReadInput(day, &input));
  absl::StatusOr<std::string> part1 = day->Part1(absl::MakeSpan(input.lines));
  if ((ret.part1_solved = part1.ok()))
    ret.part1 = *std::move(part1);
  else
    ret.part1 = absl::StrCat("Error: ", part1.status().message());
  absl::StatusOr<std::string> part2 = day->Part2(absl::MakeSpan(input.lines));
  if ((ret.part2_solved = part2.ok()))
    ret.part2 = *std::move(part2);
  else
    ret.part2 = absl::StrCat("Error: ", part2.status().message());
  ret.time = absl::Now() - start;
  return ret;
}

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

  Table() {
    switch (absl::GetFlag(FLAGS_color)) {
      case kNone:
        use_color_ = false;
        break;
      case kForce:
        use_color_ = true;
        break;
      case kAuto:
        use_color_ = IsColorTerminal();
        break;
    }
  }

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

std::vector<int> Table::Layout() const {
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
  for (int cur_span : spans_vec) {
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
            // TODO(@monkeynova): This takes a budget and adds it to each
            // column evenly. When we have different lengthts to start with
            // we should prefer evening those lenghts out first.
            int delta = need - have;
            int add_per_col = delta / cell.span;
            int change_at = col_idx + cell.span;
            if (delta % cell.span != 0) {
              change_at = col_idx + delta % cell.span;
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

std::string Table::Render() const {
  std::string ret;

  std::vector<int> col_widths = Layout();
  for (int i = 0; i < rows_.size(); ++i) {
    if (std::holds_alternative<Breaker>(rows_[i])) {
      RenderBreaker(&ret, col_widths, i);
    } else if (std::holds_alternative<std::vector<Cell>>(rows_[i])) {
      ret.append(1, '|');
      const std::vector<Cell>& cells = std::get<std::vector<Cell>>(rows_[i]);
      int col_idx = 0;
      for (const Cell& cell : cells) {
        int width = 0;
        for (int i = col_idx; i < col_idx + cell.span; ++i) {
          if (i > col_idx) width += 3;  // Margin.
          width += col_widths[i];
        }
        ret.append(1, ' ');
        cell.Render(&ret, width, use_color_);
        ret.append(" |");
        col_idx += cell.span;
      }
    }
    ret.append("\n");
  }
  return ret;
}

void Table::RenderBreaker(std::string* out, const std::vector<int> col_widths,
                          int row_num) const {
  std::vector<int> prev;
  if (row_num > 0 &&
      std::holds_alternative<std::vector<Cell>>(rows_[row_num - 1])) {
    int col_idx = 0;
    for (const Cell& cell : std::get<std::vector<Cell>>(rows_[row_num - 1])) {
      prev.push_back(col_idx);
      col_idx += cell.span;
    }
    prev.push_back(col_idx);
  }
  std::vector<int> next;
  if (row_num < rows_.size() - 1 &&
      std::holds_alternative<std::vector<Cell>>(rows_[row_num + 1])) {
    int col_idx = 0;
    for (const Cell& cell : std::get<std::vector<Cell>>(rows_[row_num + 1])) {
      next.push_back(col_idx);
      col_idx += cell.span;
    }
    next.push_back(col_idx);
  }

  auto prev_it = prev.begin();
  auto next_it = next.begin();
  for (int i = 0; i < col_widths.size(); ++i) {
    bool col_match = false;
    if (prev_it != prev.end() && *prev_it == i) {
      col_match = true;
      ++prev_it;
    }
    if (next_it != next.end() && *next_it == i) {
      col_match = true;
      ++next_it;
    }
    out->append(1, col_match ? '+' : '-');
    out->append(col_widths[i] + 2, '-');
  }
  bool col_match = false;
  if (prev_it != prev.end() && *prev_it == col_widths.size()) {
    col_match = true;
    ++prev_it;
  }
  if (next_it != next.end() && *next_it == col_widths.size()) {
    col_match = true;
    ++next_it;
  }
  CHECK(prev_it == prev.end());
  CHECK(next_it == next.end());
  out->append(1, col_match ? '+' : '-');
}

void Table::Cell::Render(std::string* out, int width, bool enable_color) const {
  bool need_reset = false;
  if (enable_color) {
    if (bold) {
      out->append("\u001b[1m");
      need_reset = true;
    }
    switch (color) {
      case kWhite:
        break;
      case kRed: {
        out->append("\u001b[31m");
        need_reset = true;
        break;
      }
      case kGreen: {
        out->append("\u001b[32m");
        need_reset = true;
        break;
      }
      case kYellow: {
        out->append("\u001b[33m");
        need_reset = true;
        break;
      }
    }
  }
  switch (justify) {
    case kRight: {
      out->append(width - entry.size(), ' ');
      out->append(entry);
      break;
    }
    case kLeft: {
      out->append(entry);
      out->append(width - entry.size(), ' ');
      break;
    }
    case kCenter: {
      int left = (width - entry.size()) / 2;
      int right = width - entry.size() - left;
      out->append(left, ' ');
      out->append(entry);
      out->append(right, ' ');
      break;
    }
  }
  if (need_reset) {
    out->append("\u001b[0m");
  }
}

Table::Cell::Color DayTimeColor(absl::Duration d) {
  if (d < absl::Milliseconds(1)) return Table::Cell::kGreen;
  if (d < absl::Milliseconds(10)) return Table::Cell::kWhite;
  if (d < absl::Milliseconds(100)) return Table::Cell::kYellow;
  return Table::Cell::kRed;
};

Table::Cell::Color YearTimeColor(absl::Duration d) {
  if (d < absl::Seconds(1)) return Table::Cell::kGreen;
  if (d < absl::Seconds(5)) return Table::Cell::kWhite;
  if (d < absl::Seconds(10)) return Table::Cell::kYellow;
  return Table::Cell::kRed;
};

std::string TimeString(absl::Duration d) {
  return d != absl::Seconds(0) ? absl::StrCat(d) : "";
};

}  // namespace

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
  table.AddRow({Table::Cell{.entry = absl::StrCat("Advent of Code ", year),
                            .span = 6,
                            .bold = true,
                            .justify = Table::Cell::kCenter}});
  table.AddBreaker();
  table.AddRow(
      {Table::Cell{.entry = "Title", .justify = Table::Cell::kCenter},
       Table::Cell{
           .entry = "Part 1", .span = 2, .justify = Table::Cell::kCenter},
       Table::Cell{
           .entry = "Part 2", .span = 2, .justify = Table::Cell::kCenter},
       Table::Cell{.entry = "Time", .justify = Table::Cell::kCenter}});
  table.AddBreaker();

  for (const DayRun& run : runs) {
    table.AddRow({Table::Cell{.entry = run.title},
                  Table::Cell{.entry = run.part1},
                  Table::Cell{.entry = run.part1_solved ? "*" : " ",
                              .color = Table::Cell::kYellow},
                  Table::Cell{.entry = run.part2},
                  Table::Cell{.entry = run.part2_solved ? "*" : " ",
                              .color = Table::Cell::kYellow},
                  Table::Cell{.entry = TimeString(run.time),
                              .color = DayTimeColor(run.time),
                              .justify = Table::Cell::kRight}});
  }
  table.AddBreaker();
  table.AddRow({Table::Cell{.entry = "Total", .span = 5},
                Table::Cell{.entry = TimeString(total_time),
                            .color = YearTimeColor(total_time),
                            .justify = Table::Cell::kRight}});
  table.AddBreaker();

  std::cout << table.Render();
}
