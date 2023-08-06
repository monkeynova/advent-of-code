load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "file_util",
    srcs = ["file_util.cc"],
    hdrs = ["file_util.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
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
        "@com_google_absl//absl/flags:flag",
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
        "@com_google_absl//absl/debugging:failure_signal_handler",
        "@com_google_absl//absl/flags:commandlineflag",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:reflection",
        "@com_google_absl//absl/functional:bind_front",
        "@com_google_absl//absl/strings",
        "@com_google_file_based_test_driver//file_based_test_driver",
        "@com_google_file_based_test_driver//file_based_test_driver:run_test_case_result",
        "@com_google_file_based_test_driver//file_based_test_driver:test_case_options",
        "@com_googlesource_code_re2//:re2",
        "@com_monkeynova_gunit_main//:test_main",
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
        "@com_google_absl//absl/status:statusor",
        "@com_google_file_based_test_driver//file_based_test_driver:test_case_options",
        "@com_googlesource_code_re2//:re2",
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
        "@com_github_google_benchmark//:benchmark",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)
