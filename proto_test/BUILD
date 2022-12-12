load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")

load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

proto_library(
    name = "msg_proto",
    srcs = ["msg.proto"],
)

cc_proto_library(
    name = "msg_proto_cc",
    deps = [":msg_proto"],
)

cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [":msg_proto_cc"],
)

proto_library(
    name = "helloworld_proto",
    srcs = ["helloworld.proto"],
    deps = [
        "@com_google_protobuf//:descriptor_proto"
    ],
)

cc_proto_library(
    name = "helloworld_proto_cc",
    deps = [":helloworld_proto"],
)

cc_grpc_library(
    name = "helloworld_grpc_proto_cc",
    srcs = [":helloworld_proto"],
    grpc_only = True,
    deps = [":helloworld_proto_cc"],
)

cc_binary(
    name = "helloworld_server",
    srcs = ["helloworld_server.cpp"],
    deps = [
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        ":helloworld_grpc_proto_cc",
    ],
)

cc_binary(
    name = "helloworld_client",
    srcs = ["helloworld_client.cpp"],
    deps = [
        "@com_github_grpc_grpc//:grpc++",
        ":helloworld_grpc_proto_cc",
    ],
)
