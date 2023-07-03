# Dependencies

This page describes the dependencies of `licensecc` and the supported build environments.

Dependencies varies with the environment, if you're building the library for the first time we suggest you download or set up one of the supported environments to be sure not to incur in dependency/compiler errors (you can use virtual machines , docker or lxc/lxd technologies).

The library is composed by two modules: a license generator `lcc` executable, that also works as a project configuration tool, and the C++ library itself `licensecc` (the part you have to integrate in your application).
 
## Library `licensecc` dependencies
These are the dependencies of the library you have to link to your code. We try to keep them at a minimum. N.B. boost is always optional and it's never linked to your application.

| Operating System               | Openssl    | Zlib                        | Boost<sup>2</sup>   |
|--------------------------------|:----------:|:---------------------------:|:-------------------:| 
| Ubuntu 20.04     | :heavy_check_mark: v1.1.1f| :question: optional         | optional(test) 1.71 |
| Ubuntu 18.04     | :heavy_check_mark: v1.1.1| :question: optional         | optional(test) 1.65 |
| Ubuntu 16.04     | :heavy_check_mark: v1.0.2| :question: req.by openssl   | optional(test)      |
| Centos 7         | :heavy_check_mark: v1.0.2| :question: req.by openssl   | otpional(test) 1.53 |
| Centos 8         | :heavy_check_mark: compile from source 1.1.1d     |    | otpional(test) 1.61 |
| Ubuntu 18.04-cross compile for Windows|recommended 1.0.2h<sup>1</sup>|    | optional(test) 1.71 |
| Windows MSVC 2017| optional<sup>1</sup>     |                             | optional(test) 1.71 |
| Windows MinGW    | optional<sup>1</sup>     |                             | optional(test) 1.71 |


## License generator executable `lccgen` dependencies

| Operating System               | Openssl      | Zlib                       | Boost<sup>3</sup>       |
|--------------------------------|:------------:|:--------------------------:|:-----------------------:| 
| Ubuntu 18.04        |:heavy_check_mark: v1.1.1|:question: if req.by openssl|:heavy_check_mark: 1.65  |
| Ubuntu 16.04        |:heavy_check_mark: v1.0.2|:question: if req.by openssl|:heavy_check_mark:       |
| Centos 7            |:heavy_check_mark: v1.0.2|:question: if req.by openssl| :heavy_check_mark: 1.53 |
| Centos 8            | :heavy_check_mark: compile from source 1.1.1d|       | :heavy_check_mark: 1.61 |
| Ubuntu 18.04-cross compile for Win|:heavy_check_mark:|                     | :heavy_check_mark: 1.71 |
| Windows MSVC 2017/2019| otpional<sup>1</sup>|                              | :heavy_check_mark: 1.71 |
| Windows MinGW         | optional<sup>1</sup>|                              | :heavy_check_mark: 1.71 |

Notes:
<sup>1</sup> If you plan to run the windows executable under Linux with wine it is necessary to use OpenSSL.

<sup>2</sup> Boost components required to run tests `unit_test_framework`,`system`,`filesystem`

<sup>3</sup> Boost is mandadory dependency of lcc executable. Components: `unit_test_framework`,`system`,`filesystem`