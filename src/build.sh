#!/bin/sh

echo "check out http://www.boost.org/doc/libs/1_46_1/doc/html/boost_asio/tutorial.html";
g++ -o foo MetaServer.cpp -lboost_system
