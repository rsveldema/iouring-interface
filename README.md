
iuring
===========================

A C++ abstraction to liburing for simplified async network-IO.

My goals:
- don't impact performance much
- present a comfortable API.
- allow easy unit testing for apps using the API

The tests/ping.cpp program illustrates how to use the library as a
client to send a request to a server and read its response.

The tests/echo_server.cpp program shows how to use the library as a
server. It waits for incoming connections
and then prints what it receives.

Prerequistes
=============================

- Linux
- cmake
    - (I used 3.31.6)
- liburing's development files installed:
    - Ubuntu:
        sudo apt install liburing-dev
    - Fedora
        sudo dnf install liburing-devel
- g++ 
    - (I used version 15)



Building and Installation
=============================

First you need to let cmake configure the build dir:
```sh
mkdir build
cmake -S . -B build
```

Then you can compile the sources:
```sh
cmake --build build
```

Finally you can install the headers and the liburing.a file:
```sh
cmake --install out/build/Debugging --prefix=${INSTALLDIR}
```

This will install the files in ${INSTALLDIR}.

Notes:
=================

- We use a minimal logging library (interface is: include/iuring/ILogger.hpp,
we implement the interface with our own logger class for the tests).

However, if you need to use a different one, all calls are through the ILogger interface
that you can implement with your own logger if needed.


