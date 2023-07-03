# Build - Linux

## Install prerequisites
Below the prerequisites for compiling `licensecc`. For developing it we use Eclipse. 
Recent CDT works smoothly with CMake. Remember to install the Ninja package as build system and Cmake Gui for a good eclipse integration.
 
### Ubuntu
Supported Ubuntu distributions are 20.04 (Focal Fossa), 18.04 (Bionic Beaver) and 16.04 (Xenial). 
It should be possible to build on any recent Debian-derivate distribution.

Install prerequisites:

```console
sudo apt-get install cmake valgrind libssl-dev zlib1g-dev libboost-test-dev libboost-filesystem-dev \
     libboost-iostreams-dev libboost-program-options-dev libboost-system-dev libboost-thread-dev \
     libboost-date-time-dev build-essential
```

For development with eclipse:

```console
sudo apt-get install cmake-gui ninja-build
```

### CentOS 7

CentOS 7 ships with gcc 4.8 that isn't compiling for a bug on regular expression. It's necessary to update to gcc 4.9 or later.
Install prerequisites:

```console
yum -y update && yum -y install install centos-release-scl
yum -y install wget boost boost-devel boost-static openssl openssl-devel openssl-static 
yum -y install glibc-static devtoolset-7-toolchain devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-valgrind-devel

export CC=/opt/rh/devtoolset-7/root/usr/bin/gcc
export CXX=/opt/rh/devtoolset-7/root/usr/bin/g++
```

Centos 7 ships with CMake 2.8.11 that's not supported. You need to compile and install a newer (>3.6) version of CMake.

```console
wget https://cmake.org/files/v3.11/cmake-3.11.0.tar.gz 
tar zxvf cmake-3.11.0.tar.gz 
cd cmake-3.11.0
./bootstrap 
make 
sudo make install
cmake --version #(check it's 3.11.0) 
```

If you don't want to install all these prerequisites in your machine you can also build the library in a docker container. 
Check for the corresponding Centos 7 section in the `.travis.yml` file at the base of the project.

### CentOS 8
Install prerequisites:

```console
yum -y update && yum -y groupinstall 'Development Tools' 
yum -y install wget cmake boost boost-devel openssl-devel zlib-devel  
dnf -y --enablerepo=PowerTools install boost-static 
```

CentOS 8 doesn't ship with a static version of openssl. It is necessary to compile it from sources.

```console
wget https://github.com/openssl/openssl/archive/OpenSSL_1_1_1d.tar.gz 
tar xzf OpenSSL_1_1_1d.tar.gz && cd openssl-OpenSSL_1_1_1d 
./config && make -j 8
sudo make install 
```

### Other linux
Licensecc should compile on any recent (2020) linux distribution. Being CentOS 7 the older distribution we keep compatibilty with. 

Minimum prerequisites
*   gcc => 4.9, cmake => 3.6
*   zlib, openssl => 1.0.2 
*   Boost => 1.57 (If you want to compile your own boost version remember to use the flag `runtime-link=static`)

Optional prerequisites:
*   Doxygen, Sphynx for documentation

## Download and compile

### Download:
This project has a submodule (the license generator). Remember to add the option `--recursive` to clone it.

```console
git clone --recursive https://github.com/open-license-manager/licensecc.git
```

### Configure:

```console
cd licensecc/build
cmake .. -DCMAKE_INSTALL_PREFIX=../install
```

### Compile and test:

```console
make
make install
```

```console
make test
ctest -T memcheck
```

### cmake useful flags

|Definition name           |Description|
|--------------------------|-----------|
|LCC_PROJECT_NAME=<str>  | This correspond to the name of the project you're generating licenses for. The flag is optional, if you don't specify it the build system will create a project named `DEFAULT` for you |
|LCC_LOCATION=<path>     | In case you download the license generator separately this is the folder where it was installed or where his lcc-config.cmake can be found |
|CMAKE_BUILD_TYPE=Release| generate a release version of the library (should be used as default)|
|CMAKE_INSTALL_PREFIX    | folder where to install compiled libraries and headers. (default: /usr/local)               |
|BOOST_ROOT              | Folder where boost was installed (optional: if you installed boost using system package manager this should not be necessary) |
|OPENSSL_ROOT            | Folder where OpenSSL was installed (optional: if you installed openssl as system package this should not be necessary) |

## Cross compile on Linux for Windows
Tested on host: Ubuntu 18.04

### Prerequisites

```console
sudo apt-get install cmake valgrind binutils-mingw-w64 mingw-w64 mingw-w64-tools \ 
	mingw-w64-x86-64-dev libz-mingw-w64-dev wine-stable wine-binfmt p7zip-full
```

Download and compile boost:

```console
export CUR_PATH=$(pwd)
wget -c https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.bz2
tar xjf boost_1_71_0.tar.bz2
rm boost_1_71_0.tar.bz2
cd boost_1_71_0
sudo ln -s /usr/bin/x86_64-w64-mingw32-g++ /usr/local/bin/g++-mingw 
./bootstrap.sh
./b2 toolset=gcc-mingw target-os=windows address-model=64 --with-date_time --with-test --with-filesystem --with-program_options --with-regex --with-serialization --with-system runtime-link=static --prefix=./dist release install
```

Install OpenSSL:

```console
wget --no-check-certificate https://bintray.com/vszakats/generic/download_file?file_path=openssl-1.0.2h-win64-mingw.7z -O openssl.7z
7z x openssl.7z
rm openssl.7z
```
Configure and compile:
 
```
cmake -DCMAKE_TOOLCHAIN_FILE=../modules/toolchain-ubuntu-mingw64.cmake -DOPENSSL_ROOT_DIR=$CUR_PATH/openssl-OpenSSL_1_1_1d/dist -DCMAKE_FIND_DEBUG_MODE=ON -DOPENSSL_USE_STATIC_LIBS=ON -DBOOST_ROOT=$CUR_PATH/boost_1_71_0/dist  ..

```

###Build documentation

Setup the python virtual environment:

```
python3 -m venv .venv

. .venv/bin/activate
pip install wheel
pip install -r requirements.txt

```

Build the docs:

```
. .venv/bin/activate
cd build
cmake ..
make documentation
```