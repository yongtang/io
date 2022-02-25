package(default_visibility = ["//visibility:public"])

cc_library(
    name = "libyaml",
    srcs = glob(
        [
            "src/*.c",
            "src/*.h",
        ],
        exclude = [
        ],
    ),
    hdrs = glob(
        [
            "include/*.h",
        ],
    ) + [
        "cmake/config.h",
    ],
    copts = [],
    defines = [
        "HAVE_CONFIG_H",
    ],
    includes = [
        "cmake",
        "include",
    ],
    deps = [
    ],
)

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
