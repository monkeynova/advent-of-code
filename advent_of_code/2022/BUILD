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
        "//advent_of_code/2022/day01:day01_benchmark_lib",
        "//advent_of_code/2022/day02:day02_benchmark_lib",
        "//advent_of_code/2022/day03:day03_benchmark_lib",
        "//advent_of_code/2022/day04:day04_benchmark_lib",
        "//advent_of_code/2022/day05:day05_benchmark_lib",
        "//advent_of_code/2022/day06:day06_benchmark_lib",
        "//advent_of_code/2022/day07:day07_benchmark_lib",
        "//advent_of_code/2022/day08:day08_benchmark_lib",
        "//advent_of_code/2022/day09:day09_benchmark_lib",
        "//advent_of_code/2022/day10:day10_benchmark_lib",
        "//advent_of_code/2022/day11:day11_benchmark_lib",
        "//advent_of_code/2022/day12:day12_benchmark_lib",
        "//advent_of_code/2022/day13:day13_benchmark_lib",
        "//advent_of_code/2022/day14:day14_benchmark_lib",
        "//advent_of_code/2022/day15:day15_benchmark_lib",
        "//advent_of_code/2022/day16:day16_benchmark_lib",
        "//advent_of_code/2022/day17:day17_benchmark_lib",
        "//advent_of_code/2022/day18:day18_benchmark_lib",
        "//advent_of_code/2022/day19:day19_benchmark_lib",
        "//advent_of_code/2022/day20:day20_benchmark_lib",
        "//advent_of_code/2022/day21:day21_benchmark_lib",
        "//advent_of_code/2022/day22:day22_benchmark_lib",
        "//advent_of_code/2022/day23:day23_benchmark_lib",
        "//advent_of_code/2022/day24:day24_benchmark_lib",
        "//advent_of_code/2022/day25:day25_benchmark_lib",
    ],
)
