#!/bin/bash
g++ ./src/nux.cpp ./src/utils/fmt.cpp \
./src/utils/log.cpp  ./src/server.cpp \
./src/console.cpp \
-O3 -o ./nux-server -flto