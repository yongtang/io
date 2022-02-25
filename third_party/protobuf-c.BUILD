package(default_visibility = ["//visibility:public"])

cc_library(
    name = "protobuf-c",
    srcs = glob(
        [
            "protobuf-c/*.c",
            "protobuf-c/*.h",
        ],
        exclude = [
        ],
    ),
    hdrs = glob(
        [
            "include/*.h",
        ],
    ) + [
        #        "cmake/config.h",
    ],
    copts = [],
    defines = [
        #        "HAVE_CONFIG_H",
    ],
    includes = [
        ".",
    ],
    deps = [
    ],
)

"""
genrule(
    name = "config_h",
    srcs = [
        "cmake/config.h.in",
    ],
    outs = [
        "cmake/config.h",
    ],
    cmd = ("sed " +
           "-e 's/@YAML_VERSION_MAJOR@/0/g' " +
           "-e 's/@YAML_VERSION_MINOR@/2/g' " +
           "-e 's/@YAML_VERSION_PATCH@/5/g' " +
           "-e 's/@YAML_VERSION_STRING@/0.2.5/g' " +
           "$< >$@"),
)
"""
