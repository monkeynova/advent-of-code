cc_test(
    name = "benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_binary(
    name = "whole_year",
    testonly = 1,
    args = [
        "--benchmark",
        "--bechmark_file_include_filter_re=input",
        "--benchmark_flags=--benchmark_min_time=0ns",
    ],
    deps = [
        ":benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
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
    ],
)
