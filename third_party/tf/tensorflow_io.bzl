def tf_io_copts():
    return (
        [
            "-std=c++14",
            "-DNDEBUG",
        ] +
        select({
            "@bazel_tools//src/conditions:darwin": [],
            "//conditions:default": ["-pthread"],
        })
    )
