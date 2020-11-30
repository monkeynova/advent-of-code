#ifndef ADVENT_OF_CODE_2019_INT_CODE_H
#define ADVENT_OF_CODE_2019_INT_CODE_H

#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_join.h"
#include "absl/strings/string_view.h"
#include "glog/logging.h"

class IntCode {
 public:
  class InputSource {
   public:
    virtual ~InputSource() = default;
    virtual absl::StatusOr<int64_t> Fetch() = 0;
  };
  class VectorInput : public InputSource {
   public:
    VectorInput(std::vector<int64_t> input) : input_(std::move(input)) {}
    ~VectorInput() override = default;

    absl::StatusOr<int64_t> Fetch() override {
      if (input_pos_ < input_.size()) return input_[input_pos_++];
      return absl::InvalidArgumentError("Input is exhausted");
    }

   private:
    std::vector<int64_t> input_;
    int64_t input_pos_ = 0;
  };

  class OutputSink {
   public:
    virtual ~OutputSink() = default;
    virtual absl::Status Put(int64_t) = 0;
  };
  class VectorOutput : public OutputSink {
   public:
    VectorOutput(std::vector<int64_t>* output) : output_(output) {}
    ~VectorOutput() override = default;

    absl::Status Put(int64_t val) override {
      if (output_ == nullptr) {
        return absl::InvalidArgumentError("Output for null sink");
      }
      output_->push_back(val);
      return absl::OkStatus();
    }

   private:
    std::vector<int64_t>* output_;
  };

  static absl::StatusOr<IntCode> Parse(
      const std::vector<absl::string_view>& input);

  IntCode(IntCode&&) = default;
  IntCode& operator=(IntCode&&) = default;

  IntCode Clone() const { return *this; }

  bool terminated() const { return terminated_; }

  absl::Status Poke(int64_t address, int64_t value) {
    if (address < 0 || address >= codes_.size()) {
      return absl::InvalidArgumentError("Bad address");
    }
    codes_[address] = value;
    return absl::OkStatus();
  }

  absl::StatusOr<int64_t> Peek(int64_t address) const {
    if (address < 0 || address >= codes_.size()) {
      return absl::InvalidArgumentError("Bad address");
    }
    return codes_[address];
  }

  absl::Status Run(const std::vector<int64_t>& input,
                   std::vector<int64_t>* output = nullptr) {
    VectorInput input_source(input);
    VectorOutput output_sink(output);
    return Run(&input_source, &output_sink);
  }

  absl::Status Run(InputSource* input = nullptr, OutputSink* output = nullptr);

  // Runs the program until an output is produced (in which case the output
  // value is returned) or the program terminates (in which case nullopt is
  // returned).
  absl::StatusOr<absl::optional<int64_t>> RunToNextOutput(
      const std::vector<int64_t>& input) {
    VectorInput input_source(input);
    return RunToNextOutput(&input_source);
  }
  absl::StatusOr<absl::optional<int64_t>> RunToNextOutput(
      InputSource* input = nullptr);

 private:
  IntCode(std::vector<int64_t> codes, int64_t code_pos = 0, bool terminated = false)
      : codes_(std::move(codes)),
        code_pos_(code_pos),
        terminated_(terminated) {}

  IntCode(const IntCode&) = default;
  IntCode& operator=(const IntCode&) = default;

  absl::Status RunSingleOpcode(InputSource* input, OutputSink* output);

  absl::StatusOr<int64_t> LoadParameter(int64_t parameter_modes, int64_t parameter) const;
  absl::Status SaveParameter(int64_t parameter_modes, int64_t parameter, int64_t value);

  std::vector<int64_t> codes_;
  int64_t code_pos_;
  int64_t relative_base_ = 0;
  bool terminated_;
};

#endif  // ADVENT_OF_CODE_2019_INT_CODE_H