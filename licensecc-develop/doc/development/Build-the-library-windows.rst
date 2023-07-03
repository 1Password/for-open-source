#######################################
Build - Windows
#######################################

This page describes how to build the library under windows. Both MSVC compiler and MinGW are supported.

MSVC (2017~2019)
*****************

Supported Visual Studio versions are: 

* the latest Visual Studio 2019 (used in development) 
* Visual Studio 2017 used in `travis.ci <https://docs.travis-ci.com/user/reference/windows/>`_ for automated tests.

MSVC install prerequisites
============================= 
Git is of course a prerequisite, if you don't have it you can download it from `git-scm.com <https://git-scm.com/download/win>`_. 

Pre-compiled `versions of boost <https://sourceforge.net/projects/boost/files/boost-binaries/>`_ for windows are available 
at SourceForge. Choose the version that matches the desired architecture (eg. for Visual Studio 2019 64 bit you can download
`boost 1.71 msvc-14.2 <https://dl.bintray.com/boostorg/release/1.71.0/binaries/boost_1_71_0-msvc-14.2-64.exe>`_ ) or 
if unsure download the full `boost_1_71_0-bin-msvc-all-32-64.7z <https://dl.bintray.com/boostorg/release/1.71.0/binaries/boost_1_71_0-bin-msvc-all-32-64.7z>`_ archive. 

With boost 1.71 it is recommended to download one of the `latest cmake <https://github.com/Kitware/CMake/releases/download/v3.17.0/cmake-3.17.0-win64-x64.msi>`_ (>3.16). The version with Visual Studio 2019 isn't the most up to date.

Checkout the code
==================
Check out the code using git:

.. NOTE::
 
  This project has a submodule (the license generator). Remember to add the option `--recursive` to clone it.

.. code-block:: console

  git clone --recursive https://github.com/open-license-manager/licensecc.git

Compile and build (command line)
====================================

Open a command prompt in the folder where you checked out the library.

Configure the library (windows x64):
 
.. code-block:: console
  
  cd build
  cmake .. -G "Visual Studio 16 2019" -A x64 -DBOOST_ROOT="C:\local\boost"  //(or where boost was installed)

Configure the library (windows x86):

For some configuration reason we're unable to build using x86 using visual studio generators. We recommend to use Ninja
build system. 

.. code-block:: console

  cd build
  "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
  cmake .. -G "Ninja" -DBOOST_ROOT="C:\local\boost"  //(or where boost was installed)

Supported cmake definitions/options
=======================================
Below a list of some useful cmake configurations:

============================== ==============================================================================================
Definition name                Description
============================== ==============================================================================================
-DSTATIC_RUNTIME=1             link statically to the standard and runtime support libraries (compiler flag /MT)
-DCMAKE_BUILD_TYPE=Release     link to the release version of the boost libraries
-DCMAKE_INSTALL_PREFIX=C:\..   folder where to install libraries and headers 
-DBOOST_ROOT=C:\..             folder where boost is installed. If cmake is reporting boost not found consider updating cmake. 
============================== ==============================================================================================

Compile and test 

.. code-block:: console
  
  cmake --build . --target install --config Release
  ctest -C Release


Compile and build (Visual studio 2019)
==========================================

Visual Studio 2019 integrates with CMake (the process requires a couple of restarts and it's all but "fluid").

After opening the project "as a CMake project" a CMakeSettings.json should appear in the base folder. Edit the file as 
below (the file is for an x86 architecture). In a special way:

* remove the "-v" switch from "buildCommandArgs" 
* add the variable BOOST_ROOT pointing to where your boost installation is.

Restart, delete and rebuild the cache a couple of time, until Visual Studio understands the new options 
(tested with Visual Studio 2019 16.5.1).

.. literalinclude:: CMakeSettings.json
   :language: json
   

MINGW
*****************

.. TODO::
   
   Describe how to install and configure mingw

Prerequistites:

* Powershell
* 7zip
* git
* cmake

Install and compile boost:

.. code-block:: console

    wget https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.7z
    7z x boost_1_68_0.7z -oC:/local
    cd "C:\local\boost_1_68_0"
    bootstrap.bat gcc 
    b2.exe -d0 --with-date_time --with-test --with-filesystem --with-program_options --with-regex --with-serialization --with-system runtime-link=static toolset=gcc --prefix=C:\local\boost_1_68_0\boost-mingw install
    cd C:/local/boost_1_68_0/boost-mingw/lib
    dir 

Verify boost is really compiled. Go to the folder where you want to download `licensecc` 

.. code-block:: console

   git clone --recursive https://github.com/open-license-manager/licensecc.git
   cd licensecc/build && cmake -G "MinGW Makefiles" -DBOOST_ROOT="C:/local/boost_1_68_0/boost-mingw" -DBoost_ARCHITECTURE="-x64" -DCMAKE_CXX_COMPILER_ARCHITECTURE_ID="x64" -DCMAKE_SH="CMAKE_SH-NOTFOUND" ..
   cmake --build . --target install --config Release

And then you can test it:
   
.. code-block:: console

   ctest -C Release
    


