cc_binary(
    name = "whole_year",
    testonly = 1,
    srcs = ["whole_year.cc"],
    deps = [
        ":2023",
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
        ":2023",
        "//advent_of_code:advent_day",
        "//advent_of_code/infra:file_util",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "2023",
    testonly = 1,
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code/2023/day01",
        "//advent_of_code/2023/day02",
        "//advent_of_code/2023/day03",
        "//advent_of_code/2023/day04",
        "//advent_of_code/2023/day05",
        "//advent_of_code/2023/day06",
        "//advent_of_code/2023/day07",
        "//advent_of_code/2023/day08",
        "//advent_of_code/2023/day09",
        "//advent_of_code/2023/day10",
        "//advent_of_code/2023/day11",
        "//advent_of_code/2023/day12",
        "//advent_of_code/2023/day13",
        "//advent_of_code/2023/day14",
        "//advent_of_code/2023/day15",
        "//advent_of_code/2023/day16",
        "//advent_of_code/2023/day17",
        "//advent_of_code/2023/day18",
        "//advent_of_code/2023/day19",
        "//advent_of_code/2023/day20",
        "//advent_of_code/2023/day21",
        "//advent_of_code/2023/day22",
        "//advent_of_code/2023/day23",
        "//advent_of_code/2023/day24",
        "//advent_of_code/2023/day25",
    ],
)
