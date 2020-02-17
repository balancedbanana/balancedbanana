Master [![Build Status](https://travis-ci.org/balancedbanana/balancedbanana.svg?branch=master)](https://travis-ci.org/balancedbanana/balancedbanana)[![Coverage Status](https://coveralls.io/repos/github/balancedbanana/balancedbanana/badge.svg?branch=master)](https://coveralls.io/github/balancedbanana/balancedbanana?branch=master)

database [![Build Status](https://travis-ci.org/balancedbanana/balancedbanana.svg?branch=database)](https://travis-ci.org/balancedbanana/balancedbanana)[![Coverage Status](https://coveralls.io/repos/github/balancedbanana/balancedbanana/badge.svg?branch=database)](https://coveralls.io/github/balancedbanana/balancedbanana?branch=database)

Testing [![Build Status](https://travis-ci.org/balancedbanana/balancedbanana.svg?branch=Testing)](https://travis-ci.org/balancedbanana/balancedbanana)[![Coverage Status](https://coveralls.io/repos/github/balancedbanana/balancedbanana/badge.svg?branch=Testing)](https://coveralls.io/github/balancedbanana/balancedbanana?branch=Testing)

communication [![Build Status](https://travis-ci.org/balancedbanana/balancedbanana.svg?branch=communication)](https://travis-ci.org/balancedbanana/balancedbanana)[![Coverage Status](https://coveralls.io/repos/github/balancedbanana/balancedbanana/badge.svg?branch=communication)](https://coveralls.io/github/balancedbanana/balancedbanana?branch=communication)

# Dev Setup
## Ubuntu 18.04
- `git clone https://github.com/balancedbanana/balancedbanana.git`
- `cd balancedbanana`
- `pip install --user cpp-coveralls`
- `sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test`
- `sudo apt-get -q update`
- `sudo apt-get -y install libssl-dev qtbase5-dev make g++-9 libqt5sql5-mysql mysql-server tar wget docker`
- `wget https://github.com/Kitware/CMake/releases/download/v3.16.2/cmake-3.16.2-Linux-x86_64.tar.gz`
- `tar -xf cmake-3.16.2-Linux-x86_64.tar.gz`
- Setup Database once if mysql was never installed before
    - `sudo systemctl start mysql`
    - `cat balancedbanana.sql | sudo mysql`
    - `echo "ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'password';"$'\n'"ALTER USER 'root'@'localhost' IDENTIFIED BY 'banana';"$'\n'"FLUSH PRIVILEGES;" | sudo mysql`
- Setup Docker for Tests
    - `sudo docker pull centos`
- Building
    - `CXX=/usr/bin/g++-9 CC=/usr/bin/gcc-9 ./cmake-3.16.2-Linux-x86_64/bin/cmake -DCOVERAGE=1 .`
    - `./cmake-3.16.2-Linux-x86_64/bin/cmake --build .`
