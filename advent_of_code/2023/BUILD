cc_test(
    name = "benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

sh_binary(
    name = "whole_year",
    testonly = 1,
    srcs = ["//tools:whole_year.sh"],
    args = ["$(location :benchmark)"],
    data = [":benchmark"],
)

cc_binary(
    name = "whole_year_bin",
    testonly = 1,
    srcs = ["whole_year.cc"],
    args = [
        "--benchmark",
        "--benchmark_flags=" +
            "--benchmark_min_time=0ns," +
            "--benchmark_filter=WholeYear," +
            "--benchmark_time_unit=us",
    ],
    deps = [
        ":benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ]
)


cc_library(
    name = "benchmark_lib",
    testonly = 1,
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code/2023/day01:day01_benchmark_lib",
        "//advent_of_code/2023/day02:day02_benchmark_lib",
        "//advent_of_code/2023/day03:day03_benchmark_lib",
        "//advent_of_code/2023/day04:day04_benchmark_lib",
        "//advent_of_code/2023/day05:day05_benchmark_lib",
    ],
)
