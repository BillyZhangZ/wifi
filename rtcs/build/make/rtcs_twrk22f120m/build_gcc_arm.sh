#!/usr/bin/env bash

make TOOL=gcc_arm CONFIG=debug build -j3 
make TOOL=gcc_arm CONFIG=release build -j3 

