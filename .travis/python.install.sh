#/bin/bash
set -x -e

apt-get -y -qq update
apt-get -y -qq install lsb-core
if [[ $(lsb_release -r | awk '{ print $2 }') == "14.04" ]]; then
  apt-get -y -qq install libav-tools
  if [[ ${1} == "2.7" ]]; then
    rm -f /usr/local/bin/pip
    ln -s /usr/local/bin/pip2 /usr/local/bin/pip
    pip install auditwheel==1.5.0
  elif [[ ${1} == "3.4" ]]; then
    rm -f /usr/local/bin/pip
    ln -s /usr/local/bin/pip3 /usr/local/bin/pip
    rm -f /usr/bin/python
    ln -s /usr/bin/python3 /usr/bin/python
    pip install auditwheel==1.5.0
  elif [[ ${1} == "3.5" ]]; then
    curl -sOL https://raw.githubusercontent.com/tensorflow/tensorflow/v1.12.0/tensorflow/tools/ci_build/install/install_python3.5_pip_packages.sh
    chmod +x install_python3.5_pip_packages.sh
    sed -i 's/apt-get update/apt-get -y -qq update/g' install_python3.5_pip_packages.sh
    sed -i 's/apt-get install/apt-get -y -qq install/g' install_python3.5_pip_packages.sh
    sed -i 's/pip3.5 install/pip3.5 -q install/g' install_python3.5_pip_packages.sh
    ./install_python3.5_pip_packages.sh
    rm -f install_python3.5_pip_packages.sh 
    rm -f /usr/bin/python
    ln -s /usr/bin/python3.5 /usr/bin/python
    rm -f /usr/local/bin/pip
    ln -s /usr/local/bin/pip3.5 /usr/local/bin/pip
  elif [[ ${1} == "3.6" ]]; then
    rm -f /usr/local/bin/pip3
    curl -sOL https://raw.githubusercontent.com/tensorflow/tensorflow/v1.12.0/tensorflow/tools/ci_build/install/install_python3.6_pip_packages.sh
    chmod +x install_python3.6_pip_packages.sh
    sed -i 's/apt-get update/apt-get -y -qq update/g' install_python3.6_pip_packages.sh
    sed -i 's/apt-get install/apt-get -y -qq install/g' install_python3.6_pip_packages.sh
    sed -i 's/apt-get upgrade/apt-get -y -qq upgrade/g' install_python3.6_pip_packages.sh
    sed -i 's/pip3 install/pip3 -q install/g' install_python3.6_pip_packages.sh
    sed -i 's/tar xvf/tar xf/g' install_python3.6_pip_packages.sh
    sed -i 's/configure/configure -q/g' install_python3.6_pip_packages.sh
    sed -i 's/make altinstall/make altinstall>make.log/g' install_python3.6_pip_packages.sh
    sed -i 's/wget /wget -q /g' install_python3.6_pip_packages.sh
    ./install_python3.6_pip_packages.sh
    rm -f install_python3.6_pip_packages.sh make.log
    rm -rf Python-3.6.1*
    rm -f /usr/bin/python
    ln -s /usr/local/bin/python3.6 /usr/bin/python
    rm -f /usr/local/bin/pip
    ln -s /usr/local/bin/pip3 /usr/local/bin/pip
  else
    echo Python ${1} not supported!
    exit 1
  fi
else
  echo Platform $(lsb_release -r | awk '{ print $2 }') not supported!
fi
python --version
pip --version
pip freeze
