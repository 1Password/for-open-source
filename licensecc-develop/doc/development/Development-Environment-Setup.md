# Developer's environment setup
This guide is just an help in case you haven't decided your development environment or your development environment match ours. 
We don't have any specific dependency on development tools, you can choose the one you prefer. Though if you want to contribute
you're required to  format the code using `clang-format` before you submit the pull request.
We use Eclipse CDT to develop the software under Linux (Ubuntu) and Visual Studio under Windows. 

## Linux setup

First of all download the source code and compile it from command line as described in [build the library](Build-the-library)
Download prerequisites:

```
sudo curl -L "https://github.com/google/styleguide/blob/gh-pages/cpplint/cpplint.py" -o /usr/bin/cpplint.py
sudo chmod a+x /usr/local/bin/cpplint.py
sudo apt-get install clang-format ninja-build
```
### Eclipse
Eclipse CDT has recently improved the support for Cmake projects, be sure to install the latest CDT version from the
marketplace, with cmake support. Remember to install `ninja` build system for integration with cmake.

Select New > C++ project > "Empty or existing cmake project" (then point it to the location where you downloaded 
the source code).

If you want to submit contributions you must install plugins that are able to use clang-format to format the code, 
or manually format the code before you submit. We suggest [CppStyle](http://www.cppstyle.com/) (available also from marketplace)

It is also necessary to install clang-format and cpplint.py (from google). 

Go to Window > Preferences > C++ > CppStyle and configure the path for `cpplint.py` you just downloaded.
Go to Project > Properties > C++ General > Formatter and select CppStyle as code formatter.

## Windows setup
(To be done ...)