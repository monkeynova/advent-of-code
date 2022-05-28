workspace(name="com_monkeynova_advent_of_code")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "bazel_skylib",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
    # branch = "main",
    commit = "312bccd83b1364fa736dde97ccba3d2b40cdfabc",
)
load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
bazel_skylib_workspace()

git_repository(
   name = "com_monkeynova_base_workspace",
   remote = "https://github.com/monkeynova/base-workspace.git",
   # branch = "main",
   commit = "6e5d9f0dde4f3882b9a3bf48ca0f7adb169d9ba2",
)

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    # branch = "main",
    commit = "9d21db9e0a60a1ea61ec19331c9bc0dd33e907b1",
)

git_repository(
    name = "com_google_absl",
    remote = "https://github.com/abseil/abseil-cpp.git",
    commit = "48f72c227b94b06387106f71d4450b31e88e283b",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "https://github.com/gflags/gflags.git",
    commit = "986e8eed00ded8168ef4eaa6f925dc6be50b40fa",
)

git_repository(
    name = "com_google_benchmark",
    remote = "https://github.com/google/benchmark.git",
    commit = "e991355c02b93fe17713efe04cbc2e278e00fdbd",
)

git_repository(
    name = "com_google_glog",
    remote = "https://github.com/google/glog.git",
    commit = "b33e3bad4c46c8a6345525fd822af355e5ef9446",
)

git_repository(
    name = "com_monkeynova_gunit_main",
    remote = "https://github.com/monkeynova/gunit-main.git",
    commit = "0123eef05b595cf41eb180f826304d9825789088",
)

git_repository(
    name = "com_googlesource_code_re2",
    remote = "https://github.com/google/re2.git",
    # branch = "abseil"
    commit = "6260193e468194afca94e6717ec76bced9b507a8",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "https://github.com/google/file-based-test-driver.git",
    commit = "fd7661b168f640f68da39f97dad26e426eb6c339",
)
