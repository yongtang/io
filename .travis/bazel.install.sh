# Install bazel
set -e -x

BAZEL_OS=${1}
BAZEL_VERSION=${2}
curl -sOL https://github.com/bazelbuild/bazel/releases/download/${BAZEL_VERSION}/bazel-${BAZEL_VERSION}-installer-${BAZEL_OS}-x86_64.sh
chmod +x bazel-${BAZEL_VERSION}-installer-${BAZEL_OS}-x86_64.sh
./bazel-${BAZEL_VERSION}-installer-${BAZEL_OS}-x86_64.sh
rm -f bazel-${BAZEL_VERSION}-installer-${BAZEL_OS}-x86_64.sh
