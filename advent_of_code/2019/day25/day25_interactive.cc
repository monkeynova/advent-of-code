#include "advent_of_code/2019/int_code.h"
#include "advent_of_code/file_util.h"
#include "glog/logging.h"
#include "main_lib.h"

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

int main(int argc, char** argv) {
  std::vector<char*> args = InitMain(argc, argv);
  if (args.size() != 2) {
    LOG(FATAL) << "Usage:\n" << args[0] << " <file>";
  }

  std::string data;
  if (absl::Status st = GetContents(args[1], &data); !st.ok()) {
    LOG(FATAL) << st.message();
  }

  std::vector<absl::string_view> tmp = {absl::string_view(data)};
  absl::StatusOr<IntCode> codes = IntCode::Parse(absl::MakeSpan(tmp));
  if (!codes.ok()) LOG(FATAL) << codes.status().message();

  Terminal t;
  if (absl::Status st = codes->Run(&t); !st.ok()) {
    LOG(FATAL) << st.message();
  }
  return 0;
}