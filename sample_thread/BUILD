load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
  name = "test",
  srcs = ["test_thread.cpp"],
  linkopts = ["-pthread"],
  copts = ["-std=c++11"],
)

cc_binary(
  name = "test2",
  srcs = ["test_thread2.cpp"],
  copts = ["-std=c++11"],
)