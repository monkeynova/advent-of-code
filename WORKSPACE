workspace(name="com_monkeynova_advent_of_code")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "bazel_skylib",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
    # branch = "main",
    commit = "60abca8fa2f970d4899857a46cb0586338dbf7a1",
)
load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
bazel_skylib_workspace()

git_repository(
   name = "com_monkeynova_base_workspace",
   remote = "https://github.com/monkeynova/base-workspace.git",
   # branch = "main",
   commit = "21c3632e277633820f20a81b084e3c764c243a97",
)

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    # branch = "main",
    commit = "912db742531bf82efb01194bc08140416e3b3467",
)

git_repository(
    name = "com_google_absl",
    remote = "https://github.com/abseil/abseil-cpp.git",
    commit = "1b976982d8cdbfd8320f34209b065bae7adefb09",
)

git_repository(
    name = "com_github_google_benchmark",
    remote = "https://github.com/google/benchmark.git",
    # branch = "main",
    commit = "2257fa4d6afb8e5a2ccd510a70f38fe7fcdf1edf",
)

git_repository(
    name = "com_googlesource_code_re2",
    remote = "https://github.com/google/re2.git",
    # branch = "abseil"
    commit = "9d65debf47ccf68e51857009d14845f0354b852c",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "https://github.com/google/file-based-test-driver.git",
    commit = "fd7661b168f640f68da39f97dad26e426eb6c339",
)
