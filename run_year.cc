#include "absl/flags/flag.h"
#include "absl/flags/usage.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "absl/time/time.h"
#include "advent_of_code/advent_day.h"
#include "advent_of_code/infra/run_day.h"
#include "advent_of_code/infra/text_table.h"
#include "main_lib.h"

ABSL_FLAG(std::string, year, "current",
          "Year to run. Use 'all' to run all years.");

namespace advent_of_code {
namespace {

std::string TimeString(absl::Duration d) {
  if (d == absl::Seconds(0)) return "";
  return absl::StrCat(d);
};

TextTable::Cell::Color DayTimeColor(absl::Duration d) {
  using enum TextTable::Cell::Color;
  if (d < absl::Milliseconds(1)) return kGreen;
  if (d < absl::Milliseconds(10)) return kWhite;
  if (d < absl::Milliseconds(100)) return kYellow;
  return kRed;
};

TextTable::Cell::Color YearTimeColor(absl::Duration d) {
  using enum TextTable::Cell::Color;
  if (d < absl::Seconds(1)) return kGreen;
  if (d < absl::Seconds(5)) return kWhite;
  if (d < absl::Seconds(10)) return kYellow;
  return kRed;
};

int RunYear(int year) {
  std::vector<std::unique_ptr<advent_of_code::AdventDay>> days;
  for (int day = 1; day <= 25; ++day) {
    days.push_back(advent_of_code::CreateAdventDay(year, day));
  }
  std::vector<advent_of_code::DayRun> runs;
  absl::Duration total_time = absl::Seconds(0);
  for (const auto& day : days) {
    if (day == nullptr) {
      runs.push_back({
        .time = absl::Seconds(0),
        .title = "???",
        .part1 = "",
        .part1_solved = false,
        .part2 = "",
        .part2_solved = false});
      continue;
    }
    absl::StatusOr<advent_of_code::DayRun> run = RunDay(day.get());
    CHECK(run.ok()) << run.status().message() << day->test_file();
    runs.push_back(*std::move(run));
    total_time += runs.back().time;
  }

  using enum TextTable::Cell::Color;
  using enum TextTable::Cell::Justify;

  advent_of_code::TextTable table;
  table.AddBreaker();
  table.AddRow({TextTable::Cell{.entry = absl::StrCat("Advent of Code ", year),
                            .justify = kCenter,
                            .bold = true,
                            .span = 6}});
  table.AddBreaker();
  table.AddRow({TextTable::Cell{.entry = "Title", .justify = kCenter},
                TextTable::Cell{.entry = "Part 1", .justify = kCenter, .span = 2},
                TextTable::Cell{.entry = "Part 2", .justify = kCenter, .span = 2},
                TextTable::Cell{.entry = "Time", .justify = kCenter}});
  table.AddBreaker();

  for (const advent_of_code::DayRun& run : runs) {
    table.AddRow(
        {TextTable::Cell{.entry = run.title}, TextTable::Cell{.entry = run.part1},
         TextTable::Cell{.entry = run.part1_solved ? "*" : " ", .color = kYellow},
         TextTable::Cell{.entry = run.part2},
         TextTable::Cell{.entry = run.part2_solved ? "*" : " ", .color = kYellow},
         TextTable::Cell{.entry = TimeString(run.time),
                     .justify = kRight,
                     .color = DayTimeColor(run.time)}});
  }
  table.AddBreaker();
  table.AddRow({TextTable::Cell{.entry = "Total", .span = 5},
                TextTable::Cell{.entry = TimeString(total_time),
                            .justify = kRight,
                            .color = YearTimeColor(total_time)}});
  table.AddBreaker();

  std::cout << table.Render();

  return 0;
}

absl::StatusOr<std::vector<int>> GetYearsFromFlag() {
  // No one will ever run this on New Years Eve/Day...
  absl::CivilYear this_year = absl::ToCivilYear(
    absl::Now() - absl::Seconds(30 * 86400), absl::UTCTimeZone());
  std::string year_str = absl::GetFlag(FLAGS_year);
  if (year_str == "all") {
    return advent_of_code::AllAdventYears();
  }
  if (year_str == "current") {
    return std::vector<int>(1, this_year.year());
  }
  if (year_str == "last") {
    return std::vector<int>(1, this_year.year() - 1);
  }
  int year_int;
  if (!absl::SimpleAtoi(year_str, &year_int)) {
    return absl::InvalidArgumentError(
        absl::StrCat(year_str,
                     " is neither a specific year nor in {'all', 'last', "
                     "'current'}"));
  }
  if (year_int < 0) year_int += this_year.year();
  return std::vector<int>(1, year_int);
}

}  // namespace
}  // namespace advent_of_code

int main(int argc, char** argv) {
  std::vector<char*> args = InitMain(
      argc, argv,
      absl::StrCat(
          "Runs all problems for a given year and displays the results "
          "in a table. Usage:\n",
          argv[0]));
  QCHECK_EQ(args.size(), 1) << "Extra argument specified." << std::endl
                            << absl::ProgramUsageMessage();

  absl::StatusOr<std::vector<int>> years = advent_of_code::GetYearsFromFlag();
  if (!years.ok()) {
    std::cerr << years.status() << std::endl;
    return 1;
  }

  for (int year : *years) {
    int error = advent_of_code::RunYear(year);
    if (error) return error;
  }

  return 0;
}