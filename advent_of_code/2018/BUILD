load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "vm",
    srcs = ["vm.cc"],
    hdrs = ["vm.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:vlog",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "2018",
    testonly = 1,
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code/2018/day01",
        "//advent_of_code/2018/day02",
        "//advent_of_code/2018/day03",
        "//advent_of_code/2018/day04",
        "//advent_of_code/2018/day05",
        "//advent_of_code/2018/day06",
        "//advent_of_code/2018/day07",
        "//advent_of_code/2018/day08",
        "//advent_of_code/2018/day09",
        "//advent_of_code/2018/day10",
        "//advent_of_code/2018/day11",
        "//advent_of_code/2018/day12",
        "//advent_of_code/2018/day13",
        "//advent_of_code/2018/day14",
        "//advent_of_code/2018/day15",
        "//advent_of_code/2018/day16",
        "//advent_of_code/2018/day17",
        "//advent_of_code/2018/day18",
        "//advent_of_code/2018/day19",
        "//advent_of_code/2018/day20",
        "//advent_of_code/2018/day21",
        "//advent_of_code/2018/day22",
        "//advent_of_code/2018/day23",
        "//advent_of_code/2018/day24",
        "//advent_of_code/2018/day25",
    ],
)
