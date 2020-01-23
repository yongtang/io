# Description:
#   Nucleus library

licenses(["notice"])  # Apache 2.0 license

exports_files(["LICENSE"])

proto_library(
    name = "position_proto",
    srcs = [
        "@nucleus//:nucleus/protos/position.proto",
    ],
)

proto_library(
    name = "range_proto",
    srcs = [
        "@nucleus//:nucleus/protos/range.proto",
    ],
)

proto_library(
    name = "cigar_proto",
    srcs = [
        "@nucleus//:nucleus/protos/cigar.proto",
    ],
)

proto_library(
    name = "struct_proto",
    srcs = [
        "@nucleus//:nucleus/protos/struct.proto",
    ],
)

proto_library(
    name = "reference_proto",
    srcs = [
        "@nucleus//:nucleus/protos/reference.proto",
    ],
    deps = [
        ":range_proto",
    ],
)

proto_library(
    name = "reads_proto",
    srcs = [
        "@nucleus//:nucleus/protos/reads.proto",
    ],
    deps = [
        ":cigar_proto",
        ":position_proto",
        ":range_proto",
        ":reference_proto",
        ":struct_proto",
    ],
)

proto_library(
    name = "variants_proto",
    srcs = [
        "@nucleus//:nucleus/protos/variants.proto",
    ],
    deps = [
        ":cigar_proto",
        ":position_proto",
        ":reference_proto",
        ":struct_proto",
    ],
)

proto_library(
    name = "fastq_proto",
    srcs = [
        "@nucleus//:nucleus/protos/fastq.proto",
    ],
    deps = [
        ":reads_proto",
        ":variants_proto",
    ],
)

cc_proto_library(
    name = "fastq_cc_pb2",
    deps = [":fastq_proto"],
)

cc_library(
    name = "fastq_reader",
    srcs = [
        "@nucleus//nucleus/io:fastq_reader.cc",
        "@nucleus//nucleus/io:hts_path.cc",
        "@nucleus//nucleus/io:reader_base.cc",
        "@nucleus//nucleus/io:text_reader.cc",
        "@nucleus//nucleus/util:utils.cc",
    ],
    hdrs = [
        "@nucleus//nucleus/io:fastq_reader.h",
        "@nucleus//nucleus/io:hts_path.h",
        "@nucleus//nucleus/io:reader_base.h",
        "@nucleus//nucleus/io:text_reader.h",
        "@nucleus//nucleus/platform:types.h",
        "@nucleus//nucleus/util:proto_ptr.h",
        "@nucleus//nucleus/util:utils.h",
        "@nucleus//nucleus/vendor:statusor.h",
    ],
defines = [
"WIN32_MEAN_AND_LEAN",
],
    visibility = ["//visibility:public"],
    deps = [
        ":fastq_cc_pb2",
        "@com_google_absl//absl/synchronization",
        "@htslib",
        "@local_config_tf//:tf_header_lib",
    ],
)
