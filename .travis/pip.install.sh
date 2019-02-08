# Pin wheel==0.31.1 to work around issue
# https://github.com/pypa/auditwheel/issues/102
pip install -q wheel==0.31.1

# Install last working version of setuptools. This must happen before we install
# absl-py, which uses install_requires notation introduced in setuptools 20.5.
pip install -q --upgrade setuptools==39.1.0

# Install six.
pip install -q --upgrade six==1.10.0

# Install protobuf.
pip install -q --upgrade protobuf==3.6.0

# numpy needs to be installed from source to fix segfaults. See:
# https://github.com/tensorflow/tensorflow/issues/6968
# This workaround isn't needed for Ubuntu 16.04 or later.
pip install -q --no-binary=:all: --upgrade numpy==1.14.5

# Keras
pip install -q keras_applications==1.0.6 --no-deps
pip install -q keras_preprocessing==1.0.5 --no-deps

pip install auditwheel==1.5.0
