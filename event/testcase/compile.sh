#########################################################################
# File Name: compile.sh
# Author: Mengxing Liu
# Created Time: 2015-03-15
#########################################################################
#!/bin/bash

g++ $1.cpp -std=c++11 -o build/$1 -lboost_coroutine -lboost_context


