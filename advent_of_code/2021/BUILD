cc_library(
    name = "bits_expr",
    srcs = ["bits_expr.cc"],
    hdrs = ["bits_expr.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "bits_expr_test",
    srcs = ["bits_expr_test.cc"],
    deps = [
        ":bits_expr",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_test(
    name = "benchmark",
    tags = ["benchmark"],
    deps = [
        "//advent_of_code/2021/day01:day01_benchmark_lib",
        "//advent_of_code/2021/day02:day02_benchmark_lib",
        "//advent_of_code/2021/day03:day03_benchmark_lib",
        "//advent_of_code/2021/day04:day04_benchmark_lib",
        "//advent_of_code/2021/day05:day05_benchmark_lib",
        "//advent_of_code/2021/day06:day06_benchmark_lib",
        "//advent_of_code/2021/day07:day07_benchmark_lib",
        "//advent_of_code/2021/day08:day08_benchmark_lib",
        "//advent_of_code/2021/day09:day09_benchmark_lib",
        "//advent_of_code/2021/day10:day10_benchmark_lib",
        "//advent_of_code/2021/day11:day11_benchmark_lib",
        "//advent_of_code/2021/day12:day12_benchmark_lib",
        "//advent_of_code/2021/day13:day13_benchmark_lib",
        "//advent_of_code/2021/day14:day14_benchmark_lib",
        "//advent_of_code/2021/day15:day15_benchmark_lib",
        "//advent_of_code/2021/day16:day16_benchmark_lib",
        "//advent_of_code/2021/day17:day17_benchmark_lib",
        "//advent_of_code/2021/day18:day18_benchmark_lib",
        "//advent_of_code/2021/day19:day19_benchmark_lib",
        "//advent_of_code/2021/day20:day20_benchmark_lib",
        "//advent_of_code/2021/day21:day21_benchmark_lib",
        "//advent_of_code/2021/day22:day22_benchmark_lib",
        "//advent_of_code/2021/day23:day23_benchmark_lib",
        "//advent_of_code/2021/day24:day24_benchmark_lib",
        "//advent_of_code/2021/day25:day25_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
