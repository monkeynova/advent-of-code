load("@com_monkeynova_base_workspace//:default_rules.bzl", "default_rules")

default_rules(workspace_dep = "update_workspace.date")

cc_binary(
    name = "run_year",
    srcs = ["run_year.cc"],
    args = [
        "--ocr_mode=output",
        "--advent_day_run_audit=false",
    ],
    deps = [
        "//advent_of_code",
        "//advent_of_code:advent_day",
        "//advent_of_code/infra:file_util",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:usage",
        "@com_monkeynova_gunit_main//:main_lib",
    ],
)

cc_binary(
    name = "benchmark",
    testonly = 1,
    srcs = ["benchmark.cc"],
    args = [
        "--advent_day_run_audit=false",
        "--benchmark",
        "--benchmark_flags=" +
        "--benchmark_min_time=0ns," +
        "--benchmark_time_unit=us",
    ],
    deps = [
        "//advent_of_code",
        "//advent_of_code:advent_day",
        "//advent_of_code/infra:file_util",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
