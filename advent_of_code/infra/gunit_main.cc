#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/check.h"
#include "absl/log/flags.h"
#include "absl/log/initialize.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "benchmark/benchmark.h"
#include "gtest/gtest.h"

ABSL_FLAG(bool, benchmark, false,
          "If true, runs benchmarks rather than gunit test suite.");

ABSL_FLAG(std::string, benchmark_flags, "", "...");

int main(int argc, char** argv) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler(/*options=*/{});
  absl::InitializeLog();
  testing::InitGoogleTest();

  std::vector<char*> args = absl::ParseCommandLine(argc, argv);
  CHECK_EQ(args.size(), 1) << absl::StrJoin(args, ",");

  if (absl::GetFlag(FLAGS_benchmark)) {
    std::vector<std::string> benchmark_flags =
        absl::StrSplit(absl::GetFlag(FLAGS_benchmark_flags), ",");

    int benchmark_argc = benchmark_flags.size() + 1;
    std::vector<char*> benchmark_argv = {argv[0]};
    for (absl::string_view arg : benchmark_flags) {
      benchmark_argv.push_back(const_cast<char*>(arg.data()));
    }
    benchmark::Initialize(&benchmark_argc,
                          const_cast<char**>(benchmark_argv.data()));
    benchmark::RunSpecifiedBenchmarks();
    return 0;
  }
  return RUN_ALL_TESTS();
}
