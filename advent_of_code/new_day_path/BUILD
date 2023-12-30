load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "new_day",
    srcs = ["new_day.cc"],
    hdrs = ["new_day.h"],
    data = [
        "input.txt",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:bfs",
        "//advent_of_code:char_board",
        "//advent_of_code:conway",
        "//advent_of_code:directed_graph",
        "//advent_of_code:fast_board",
        "//advent_of_code:interval",
        "//advent_of_code:loop_history",
        "//advent_of_code:mod",
        "//advent_of_code:point",
        "//advent_of_code:point3",
        "//advent_of_code:point_walk",
        "//advent_of_code:splice_ring",
        "//advent_of_code:tokenizer",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "new_day_test",
    size = "small",
    srcs = ["new_day_test.cc"],
    data = [
        "new_day.test",
    ],
    deps = [
        ":new_day",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
