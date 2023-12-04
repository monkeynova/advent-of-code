#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/check.h"
#include "absl/log/flags.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "advent_of_code/2019/int_code.h"
#include "advent_of_code/infra/file_util.h"
#include "main_lib.h"

namespace advent_of_code {
namespace {

class Terminal : public IntCode::IOModule {
 public:
  bool PauseIntCode() override { return done_; }

  absl::StatusOr<int64_t> Fetch() override {
    if (input_pos_ < input_.size()) return input_[input_pos_++];
    std::getline(std::cin, input_);
    if (std::cin.eof()) {
      done_ = true;
    }
    char term[] = {'\n', '\0'};
    absl::StrAppend(&input_, term);
    input_pos_ = 0;
    return input_[input_pos_++];
  }

  absl::Status Put(int64_t val) override {
    if (val == '\n') {
      std::cout << output_ << std::endl;
      output_.clear();
    } else {
      char val_str[] = {static_cast<char>(val), '\0'};
      absl::StrAppend(&output_, val_str);
    }
    return absl::OkStatus();
  }

 private:
  bool done_ = false;
  std::string input_;
  int input_pos_;
  std::string output_;
};

int RunInteractive(int argc, char** argv) {
  std::vector<char*> args = InitMain(argc, argv);
  CHECK_EQ(args.size(), 2) << absl::StrJoin(args, ",");

  if (args.size() != 2) {
    LOG(FATAL) << "Usage:\n" << args[0] << " <file>";
  }

  absl::StatusOr<std::string> data = GetContents(args[1]);
  if (!data.ok()) {
    LOG(FATAL) << data.status();
  }

  std::vector<std::string_view> tmp = {std::string_view(*data)};
  absl::StatusOr<IntCode> codes = IntCode::Parse(absl::MakeSpan(tmp));
  if (!codes.ok()) LOG(FATAL) << codes.status().ToString();

  Terminal t;
  if (absl::Status st = codes->Run(&t); !st.ok()) {
    LOG(FATAL) << st.ToString();
  }
  return 0;
}

}  // namespace
}  // namespace advent_of_code

int main(int argc, char** argv) {
  return advent_of_code::RunInteractive(argc, argv);
}
