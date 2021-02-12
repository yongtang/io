workspace(name = "foo")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "io_bazel_rules_closure",
    sha256 = "5b00383d08dd71f28503736db0500b6fb4dda47489ff5fc6bed42557c07c6ba9",
    strip_prefix = "rules_closure-308b05b2419edb5c8ee0471b67a40403df940149",
    urls = [
        "https://storage.googleapis.com/mirror.tensorflow.org/github.com/bazelbuild/rules_closure/archive/308b05b2419edb5c8ee0471b67a40403df940149.tar.gz",
        "https://github.com/bazelbuild/rules_closure/archive/308b05b2419edb5c8ee0471b67a40403df940149.tar.gz",  # 2019-06-13
    ],
)

http_archive(
    name = "org_tensorflow",
    urls = [
        "https://github.com/tensorflow/tensorflow/archive/a7e77bb4eb7efd9fa7453328371dd0806247641c.tar.gz",
    ],
    strip_prefix = "tensorflow-a7e77bb4eb7efd9fa7453328371dd0806247641c",
    sha256 = "e478398027b2b4af7f514b2ec7de3c1b13258a3d65c719e74eebbd03c2557061",
)

load("@org_tensorflow//tensorflow:workspace.bzl", "tf_workspace")

tf_workspace(tf_repo_name = "org_tensorflow")
