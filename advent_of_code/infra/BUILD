load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "file_util",
    srcs = ["file_util.cc"],
    hdrs = ["file_util.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
        "@abseil-cpp//absl/strings",
        "@com_monkeynova_gunit_main//:vlog",
        "@re2",
    ],
)

cc_library(
    name = "file_flags",
    srcs = ["file_flags.cc"],
    hdrs = ["file_flags.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@abseil-cpp//absl/flags:flag",
    ],
    alwayslink = 1,
)

cc_library(
    name = "file_test",
    testonly = 1,
    srcs = ["file_test.cc"],
    hdrs = ["file_test.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":file_flags",
        ":file_test_options",
        ":file_util",
        "//advent_of_code:advent_day",
        "@abseil-cpp//absl/debugging:failure_signal_handler",
        "@abseil-cpp//absl/flags:commandlineflag",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:reflection",
        "@abseil-cpp//absl/functional:bind_front",
        "@abseil-cpp//absl/strings",
        "@com_google_file_based_test_driver//file_based_test_driver",
        "@com_google_file_based_test_driver//file_based_test_driver:run_test_case_result",
        "@com_google_file_based_test_driver//file_based_test_driver:test_case_options",
        "@com_monkeynova_gunit_main//:test_main",
        "@re2",
    ],
)

cc_library(
    name = "file_test_options",
    testonly = 1,
    srcs = ["file_test_options.cc"],
    hdrs = ["file_test_options.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@abseil-cpp//absl/status:statusor",
        "@com_google_file_based_test_driver//file_based_test_driver:test_case_options",
        "@re2",
    ],
)

cc_library(
    name = "file_benchmark",
    testonly = 1,
    srcs = ["file_benchmark.cc"],
    hdrs = ["file_benchmark.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":file_flags",
        ":file_test_options",
        ":file_util",
        "//advent_of_code:advent_day",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/status:statusor",
        "@abseil-cpp//absl/strings",
        "@google_benchmark//:benchmark",
    ],
)

cc_library(
    name = "run_day",
    srcs = ["run_day.cc"],
    hdrs = ["run_day.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":file_util",
        "//advent_of_code:advent_day",
        "@abseil-cpp//absl/status:statusor",
        "@abseil-cpp//absl/time",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "text_table",
    srcs = ["text_table.cc"],
    hdrs = ["text_table.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@abseil-cpp//absl/algorithm:container",
        "@abseil-cpp//absl/container:flat_hash_set",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/log:check",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)
