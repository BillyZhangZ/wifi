#!/bin/bash 
make TOOL=gcc_arm CONFIG=debug build -j6
make TOOL=gcc_arm CONFIG=release build -j6
