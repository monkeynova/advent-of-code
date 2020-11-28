cc_library(
    name = "day1",
    srcs = ["day1.cc"],
    hdrs = ["day1.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "day1_test",
    srcs = ["day1_test.cc"],
    args = ["--test_file=advent_of_code/2019/day1/day1.test"],
    data = [
        "day1.test",
    ],
    deps = [
        ":day1",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/functional:bind_front",
        "@com_google_absl//absl/strings",
        "@com_google_file_based_test_driver//:file_based_test_driver",
        "@com_google_file_based_test_driver//:run_test_case_result",
        "@com_google_file_based_test_driver//:test_case_options",
        "@com_monkeynova_gunit_main//:main_lib",
    ],
)
