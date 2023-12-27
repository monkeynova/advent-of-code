cc_binary(
    name = "whole_year",
    testonly = 1,
    srcs = ["whole_year.cc"],
    deps = [
        ":benchmark_lib",
        "//advent_of_code:advent_day",
        "//advent_of_code/infra:file_util",
        "@com_google_absl//absl/flags:flag",
        "@com_monkeynova_gunit_main//:main_lib",
    ],
)

cc_binary(
    name = "benchmark",
    testonly = 1,
    srcs = ["benchmark.cc"],
    args = [
        "--benchmark",
        "--benchmark_flags=" +
        "--benchmark_min_time=0ns," +
        "--benchmark_time_unit=us",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code/infra:file_util",
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
        "//advent_of_code/2023/day03:day03_benchmark_lib",
        "//advent_of_code/2023/day04:day04_benchmark_lib",
        "//advent_of_code/2023/day05:day05_benchmark_lib",
        "//advent_of_code/2023/day06:day06_benchmark_lib",
        "//advent_of_code/2023/day07:day07_benchmark_lib",
        "//advent_of_code/2023/day08:day08_benchmark_lib",
        "//advent_of_code/2023/day09:day09_benchmark_lib",
        "//advent_of_code/2023/day10:day10_benchmark_lib",
        "//advent_of_code/2023/day11:day11_benchmark_lib",
        "//advent_of_code/2023/day12:day12_benchmark_lib",
        "//advent_of_code/2023/day13:day13_benchmark_lib",
        "//advent_of_code/2023/day14:day14_benchmark_lib",
        "//advent_of_code/2023/day15:day15_benchmark_lib",
        "//advent_of_code/2023/day16:day16_benchmark_lib",
        "//advent_of_code/2023/day17:day17_benchmark_lib",
        "//advent_of_code/2023/day18:day18_benchmark_lib",
        "//advent_of_code/2023/day19:day19_benchmark_lib",
        "//advent_of_code/2023/day20:day20_benchmark_lib",
        "//advent_of_code/2023/day21:day21_benchmark_lib",
        "//advent_of_code/2023/day22:day22_benchmark_lib",
        "//advent_of_code/2023/day23:day23_benchmark_lib",
        "//advent_of_code/2023/day24:day24_benchmark_lib",
        "//advent_of_code/2023/day25:day25_benchmark_lib",
    ],
)
