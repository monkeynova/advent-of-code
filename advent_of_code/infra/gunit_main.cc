#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/check.h"
#include "absl/log/flags.h"
#include "absl/log/initialize.h"
#include "absl/strings/str_join.h"
#include "benchmark/benchmark.h"
#include "gtest/gtest.h"

ABSL_FLAG(bool, benchmark, false,
          "If true, runs benchmarks rather than gunit test suite.");

int main(int argc, char** argv) {
  absl::InitializeSymbolizer(argv[0]);
  absl::InstallFailureSignalHandler(/*options=*/{});
  absl::InitializeLog();
  testing::InitGoogleTest();

  {
    // Allow benchmark to read flags, but don't let it consume them.
    int consumable_argc = argc;
    std::vector<char*> consumable_argv(argc);
    for (int i = 0; argv[i] != nullptr; ++i) {
      consumable_argv.push_back(argv[i]);
    }
    benchmark::Initialize(&consumable_argc,
                          const_cast<char**>(consumable_argv.data()));
  }
  std::vector<char*> args = absl::ParseCommandLine(argc, argv);
  CHECK_EQ(args.size(), 1) << absl::StrJoin(args, ",");

  if (absl::GetFlag(FLAGS_benchmark)) {
    ::benchmark::RunSpecifiedBenchmarks();
    return 0;
  }
  return RUN_ALL_TESTS();
}
