load("@rules_cc//cc:defs.bzl", "cc_binary")

# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary
cc_binary(
  name = "main",
  srcs = glob(["*.cpp"]),
  deps = ["@com_google_protobuf//:protobuf"],
)

