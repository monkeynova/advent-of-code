load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "new_day",
    srcs = ["new_day.cc"],
    hdrs = ["new_day.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/log",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "new_day_input",
    testonly = 1,
    data = [
        "input.txt",
        "new_day.test",
    ],
    deps = [
        ":new_day",
    ],
)

cc_test(
    name = "new_day_test",
    srcs = ["new_day_test.cc"],
    deps = [
        ":new_day_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "new_day_benchmark_lib",
    testonly = 1,
    srcs = ["new_day_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":new_day_input",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:gunit_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "new_day_benchmark",
    tags = ["benchmark"],
    deps = [
        ":new_day_benchmark_lib",
        "//advent_of_code/infra:gunit_main",
    ],
)
