
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
