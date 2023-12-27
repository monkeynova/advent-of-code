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
  Table() = default;

  void JustifyRight(int col) {
    if (just_right_.size() <= col) {
      just_right_.resize(col + 1, false);
    }
    just_right_[col] = true;
  }

  void AddTitle(std::string title) {
    rows_.emplace_back(title);
  }

  void AddBreaker() {
    rows_.emplace_back(Breaker());
  }

  void AddRow(std::vector<std::string> row) {
    if (row.size() > col_widths_.size()) {
      col_widths_.resize(row.size(), 0);
      just_right_.resize(row.size(), false);
    }
    for (int i = 0; i < row.size(); ++i) {
      col_widths_[i] = std::max<int>(col_widths_[i], row[i].size());
    }
    rows_.emplace_back(std::move(row));
  }

  std::string Render() {
    std::string ret;

    std::string breaker = "+";
    for (int i = 0; i < col_widths_.size(); ++i) {
      breaker.append(col_widths_[i] + 2, '-');
      breaker.append(1, '+');
    }
    for (const auto& row : rows_) {
      if (std::holds_alternative<Breaker>(row)) {
        absl::StrAppend(&ret, breaker, "\n");
      } else if (std::holds_alternative<std::string>(row)) {
        std::string title = std::get<std::string>(row);
        ret.append(1, '|');
        int total_width = col_widths_.size() - 1;
        for (int i = 0; i < col_widths_.size(); ++i) {
          total_width += col_widths_[i] + 2;
        }
        int left = (total_width - title.size()) / 2;
        int right = total_width - title.size() - left;
        ret.append(left, ' ');
        ret.append(title);
        ret.append(right, ' ');
        ret.append("|\n");
      } else if (std::holds_alternative<std::vector<std::string>>(row)) {
        ret.append(1, '|');
        const std::vector<std::string>& cols = std::get<std::vector<std::string>>(row);
        for (int i = 0; i < col_widths_.size(); ++i) {
          ret.append(1, ' ');
          if (cols.size() <= i) {
            ret.append(col_widths_[i], ' ');
          } else {
            if (just_right_[i]) {
              ret.append(col_widths_[i] - cols[i].size(), ' ');
              ret.append(cols[i]);
            } else {
              ret.append(cols[i]);
              ret.append(col_widths_[i] - cols[i].size(), ' ');
            }
          }
          ret.append(" |");
        }
        ret.append("\n");
      }
    }

    return ret;
  }

 private:
  struct Breaker {};
  using Row = std::variant<std::string, Breaker, std::vector<std::string>>;
  std::vector<Row> rows_;
  std::vector<int> col_widths_;
  std::vector<bool> just_right_;
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
  table.AddTitle(absl::StrCat("Advent of Code ", year));
  table.AddBreaker();
  table.AddRow({"Title", "Part 1", "Part 2", "Time"});
  table.JustifyRight(3);
  table.AddBreaker();
  for (const DayRun& run : runs) {
    table.AddRow({run.title, run.part1, run.part2, absl::StrCat(run.time)});
  }
  table.AddBreaker();
  table.AddRow({"Total", "", "", absl::StrCat(total_time)});
  table.AddBreaker();

  std::cout << table.Render();
}
