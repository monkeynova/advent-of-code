#include "advent_of_code/infra/run_day.h"

#include "advent_of_code/infra/file_util.h"
#include "vlog.h"

namespace advent_of_code {

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
#ifdef _WIN32
  ret->lines = absl::StrSplit(ret->file, "\r\n");
#else
  ret->lines = absl::StrSplit(ret->file, '\n');
#endif
  while (!ret->lines.empty() && ret->lines.back().empty()) {
    ret->lines.pop_back();
  }
  return absl::OkStatus();
}

}  // namespace

absl::StatusOr<DayRun> RunDay(advent_of_code::AdventDay* day) {
  if (day == nullptr) {
    return absl::NotFoundError("Null day");
  }
  DayRun ret;
  absl::Time start = absl::Now();
  ret.title = std::string(day->title());
  Input input;
  RETURN_IF_ERROR(ReadInput(day, &input));
  ret.bytes_processed = input.file.size();
  absl::StatusOr<std::string> part1 = day->Part1(absl::MakeSpan(input.lines));
  if (part1.ok()) {
    ret.part1 = *std::move(part1);
    ret.part1_solved = true;
  } else {
    ret.part1 = absl::StrCat("Error: ", part1.status().message());
    ret.part1_solved = false;
  }
  absl::StatusOr<std::string> part2 = day->Part2(absl::MakeSpan(input.lines));
  if (part2.ok()) {
    ret.part2 = *std::move(part2);
    ret.part2_solved = true;
  } else {
    ret.part2 = absl::StrCat("Error: ", part2.status().message());
    ret.part2_solved = false;
  }
  ret.time = absl::Now() - start;
  return ret;
}

}  // namespace advent_of_code
