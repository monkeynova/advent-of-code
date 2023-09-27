workspace(name="com_monkeynova_advent_of_code")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "bazel_skylib",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
    # branch = "main",
    commit = "652c8f0b2817daaa2570b7a3b2147643210f7dc7",
)
load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
bazel_skylib_workspace()

git_repository(
   name = "com_monkeynova_base_workspace",
   remote = "https://github.com/monkeynova/base-workspace.git",
   # branch = "main",
   commit = "079048a672da87d2cede89febac49f54a5cbb165",
)

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    # branch = "main",
    commit = "e47544ad31cb3ceecd04cc13e8fe556f8df9fe0b",
)

git_repository(
    name = "com_google_absl",
    remote = "https://github.com/abseil/abseil-cpp.git",
    commit = "bab9544a35615c1cdcf4271c6c52c9f1559c7d3c",
)

git_repository(
    name = "com_github_google_benchmark",
    remote = "https://github.com/google/benchmark.git",
    # branch = "main",
    commit = "7736df03049c362c7275f7573de6d6a685630e0a",
)

git_repository(
    name = "com_googlesource_code_re2",
    remote = "https://github.com/google/re2.git",
    # branch = "abseil"
    commit = "578843a516fd1da7084ae46209a75f3613b6065e",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "https://github.com/google/file-based-test-driver.git",
    commit = "fd7661b168f640f68da39f97dad26e426eb6c339",
)

git_repository(
    name = "com_monkeynova_gunit_main",
    remote = "https://github.com/monkeynova/gunit-main.git",
    # branch = "main"
    commit = "3eba1a031dd22ca8c7c1b0732a9d5c970fc68889",
)

git_repository(
    name = "aspect_bazel_lib",
    remote = "https://github.com/aspect-build/bazel-lib.git",
    # branch = "main",
    commit = "a4e997de1f946a26a5c5e3e253a74329aace9b89",
)
